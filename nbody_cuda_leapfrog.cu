#include <cuda_runtime.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "vector.hpp"
#include "body.hpp"
#include"IO.hpp"
#include <chrono>
using namespace std;

using Real = double;
const int dim = 2;
using Vec = Vector<Real, dim>;
using Particle = Body<Real, dim>;

// ===================== Compute Force =========================
template <typename T, int N>
__host__ __device__ Vector<T, N> computeForce(const Body<T, N>& a, const Body<T, N>& b)
{
    const T G = 10.0;
    Vector<T, N> r = b.getPosition() - a.getPosition();
    T dist = r.norm() + 1e-9;
    T F =- G * (a.getMass() * b.getMass()) / (dist * dist * dist);
    return F * r;
}

// ===================== CUDA Kernel: Compute Forces =========================
template <typename T, int N>
__global__ void compute_forces_kernel(
    const Body<T, N>* bodies,
    Vector<T, N>* forces,
    int numBodies)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= numBodies) return;

    Vector<T, N> totalForce;
    for (int j = 0; j < numBodies; ++j)
    {
        if (i != j)
            totalForce += computeForce(bodies[i], bodies[j]);
    }
    forces[i] = totalForce;
}

// ===================== CUDA Kernel: Update Bodies =========================
template <typename T, int N>
__global__ void update_bodies_kernel(
    Body<T, N>* bodies,
    const Vector<T, N>* forces,
    int numBodies,
    T deltaT,
    int step) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= numBodies) return;

    Body<T, N> b = bodies[i];

    Vector<T, N> acc = b.getAcceleration();
    Vector<T, N> vel = b.getVelocity();
    Vector<T, N> pos = b.getPosition();

    vel += 0.5 * acc * deltaT;
    pos += vel * deltaT;
    acc = forces[i] / b.getMass();
    vel += 0.5 * acc * deltaT;

    b.setAcceleration(acc);
    b.setVelocity(vel);
    b.setPosition(pos);

    bodies[i] = b;

    // Debug print
    
}

// ===================== Host Driver =========================
void run_simulation(std::vector<Particle>& h_bodies, Real deltaT, int steps, const std::string& output_filename)
{
    int numBodies = h_bodies.size();
    int threadsPerBlock = 256;
    int blocks = (numBodies + threadsPerBlock - 1) / threadsPerBlock;

    Particle* d_bodies;
    Vec* d_forces;

    cudaMalloc(&d_bodies, sizeof(Particle) * numBodies);
    cudaMemcpy(d_bodies, h_bodies.data(), sizeof(Particle) * numBodies, cudaMemcpyHostToDevice);

    cudaMalloc(&d_forces, sizeof(Vec) * numBodies);
    cudaMemset(d_forces, 0, sizeof(Vec) * numBodies);

    std::ofstream fout(output_filename);
    fout << "step,id,x,y\n";

    std::cout << "Launching kernel with blocks=" << blocks << ", threadsPerBlock=" << threadsPerBlock << std::endl;

    for (int step = 0; step < steps; ++step)
    {
        compute_forces_kernel<Real, dim> << <blocks, threadsPerBlock >> > (d_bodies, d_forces, numBodies);
        cudaError_t err1 = cudaGetLastError();
        if (err1 != cudaSuccess) {
            std::cerr << "compute_forces_kernel launch failed: " << cudaGetErrorString(err1) << std::endl;
        }
        update_bodies_kernel<Real, dim> << <blocks, threadsPerBlock >> > (d_bodies, d_forces, numBodies, deltaT, step);
        cudaError_t err2 = cudaGetLastError();
        if (err2 != cudaSuccess) {
            std::cerr << "update_bodies_kernel launch failed: " << cudaGetErrorString(err2) << std::endl;
        }
        cudaDeviceSynchronize();

        if (step % 10 == 0)
        {
            cudaMemcpy(h_bodies.data(), d_bodies, sizeof(Particle) * numBodies, cudaMemcpyDeviceToHost);
            write_trajectory_csv_frame<Real, dim>(fout, h_bodies, step);
        }

    }

    cudaMemcpy(h_bodies.data(), d_bodies, sizeof(Particle) * numBodies, cudaMemcpyDeviceToHost);

    fout.close();
    cudaFree(d_bodies);
    cudaFree(d_forces);
}

// ===================== Example Main =========================
int main()
{

    std::vector<Particle> bodies = read_bodies_from_txt<Real, dim>("input.txt");
    std::cout << "Read " << bodies.size() << " particles from file.\n";
    auto start = std::chrono::high_resolution_clock::now();
    run_simulation(bodies, 0.01, 10000, "trajectory.csv");
    auto end = std::chrono::high_resolution_clock::now();

    // compute timecost
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Time duration: " << duration.count() << " ms" << std::endl;
  
    print_bodies(bodies);
    

    return 0;
}
