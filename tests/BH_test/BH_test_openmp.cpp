#include "Vector.hpp"
#include "Body.hpp"
#include "Quad.hpp"
#include "BHTree.hpp"
#include "NBodyBHSolverOpenMP.hpp"
typedef NBodyBHSolverOpenMP<double> SolverType;
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <omp.h>

// Helper function to write positions to file for each timestep
template <typename T>
void writePositionsToFile(const std::vector<Body<T, 2>> &bodies, int timestep, std::ofstream &outFile)
{
    outFile << "# Timestep " << timestep << "\n";
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        const auto &pos = bodies[i].getPosition();
        const auto energy = bodies[i].getEnergy();
        outFile << i << "\t" << pos[0] << "\t" << pos[1] << "\t" << energy << "\n";
    }
    outFile << "\n";
}

int main()
{
    using T = double;

    T universeSize = 3.0;
    Vector<T, 2> origin({-universeSize / 2, -universeSize / 2});
    Quad<T> universe(origin, universeSize);

    T timeStep = 1e-4;
    SolverType solver(universe, timeStep);

    struct PlanetData
    {
        std::string name;
        T mass;
        T x;
        T y;
        T vx;
        T vy;
    };

    std::vector<PlanetData> planets;
    std::ifstream in("GeneratedBodies/generated_bodies_5000.txt");
    if (!in)
    {
        std::cerr << "Failed to open input file" << std::endl;
        return 1;
    }
    double mass, x, y, vx, vy;
    int id = 0;
    while (in >> x >> y >> vx >> vy >> mass) {
        PlanetData p;
        p.name = "P" + std::to_string(id++);
        p.mass = mass;
        p.x = x;
        p.y = y;
        p.vx = vx;
        p.vy = vy;
        planets.push_back(p);
    }

    for (const auto &planet : planets)
    {
        Vector<T, 2> position({planet.x, planet.y});
        Vector<T, 2> velocity({planet.vx, planet.vy});
        solver.addBody(Body<T, 2>(planet.mass, position, velocity));
    }

    std::ofstream outFile("../particle_positions_openmp.txt");
    if (!outFile)
    {
        std::cerr << "Failed to open output file" << std::endl;
        return 1;
    }

    const int numParticles = planets.size();
    outFile << "# Format: ParticleID\tX\tY\tE\n";
    outFile << "# Number of particles: " << numParticles << "\n\n";

    const int numSteps = 100;  // Change as needed
    std::cout << "Starting simulation with " << numParticles << " particles for " << numSteps << " steps\n";
    auto start = std::chrono::high_resolution_clock::now();

    solver.calculateEnergy();
    writePositionsToFile(solver.getBodies(), 0, outFile);

    for (int step = 1; step <= numSteps; ++step)
    {
        solver.simulateOneStep();

        if (step % 1000 == 0 || step == numSteps)
        {
            solver.calculateEnergy();
            writePositionsToFile(solver.getBodies(), step, outFile);
            std::cout << "Completed step " << step << "/" << numSteps << "\n";
        }
    }

    outFile.close();

    std::cout << "Simulation complete. Results written to particle_positions_openmp.txt\n";

    // Measure the total time taken for the simulation
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();

    int threads = omp_get_max_threads();
    int body_count = numParticles;

    // Append to CSV file
    std::ifstream checkExist("../openmp_runtime_different_threads.csv");
    bool file_exists = checkExist.good();
    checkExist.close();

    std::ofstream csvFile("../openmp_runtime_different_threads.csv", std::ios::app);
    if (csvFile.is_open()) {
        if (!file_exists) {
            csvFile << "Bodies,Threads,TimePerStep\n";
        }
        double time_per_step = duration / numSteps;
        csvFile << body_count << "," << threads << "," << time_per_step << "\n";
        csvFile.close();
    } else {
        std::cerr << "Failed to open CSV file.\n";
    }

    std::cout << "Total simulation time: " << duration << " seconds using " << threads << " OpenMP threads.\n";
    return 0;
}
