#include "../balanced_fmm_tree.hpp"
#include "../particle_generators.hpp"
#include <fstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <cmath>
#include <omp.h> // For timing

using namespace fmm;

constexpr size_t items_per_leaf = 128;
constexpr size_t d = 2;
constexpr double extent = 10.0;
constexpr double dt = 0.001;
constexpr size_t n_steps = 1000;
constexpr double fault_tolerance_eps = 0.01;
constexpr double soften_eps = 0.01;
const double G = 1.0;

using Vec = Vector_<d>;
using Src = Body_<d>;

int main()
{
    // List of N to test
    std::vector<size_t> N_list = {256, 512, 1024, 2048, 4096, 8192};

    // Prepare output file (overwrite each run)
    std::ofstream out_eff("integration_efficiency.txt");
    out_eff << "# N total_time_ms avg_time_per_step_ms\n";
    out_eff.close();

    for (size_t N : N_list)
    {
        size_t items_per_leaf = N / 64; // keep items_per_leaf proportional to N
        double total_mass = N * 1.0;
        std::vector<Src> bodies = generateUniformSquare<d>(N, extent, total_mass, 42);

        std::vector<Vec> velocities(bodies.size());
        std::vector<Vec> accelerations(bodies.size());

        for (size_t i = 0; i < bodies.size(); ++i)
            velocities[i] = Vec{{0.0, 0.0}};

        BalancedFmmTree<d> tree(bodies, items_per_leaf, fault_tolerance_eps, soften_eps);
        for (size_t i = 0; i < bodies.size(); ++i)
            accelerations[i] = tree.evaluateForcefield(bodies[i].position) / bodies[i].q;

        double t_start = omp_get_wtime();

        for (size_t step = 1; step <= n_steps; ++step)
        {
            for (size_t i = 0; i < bodies.size(); ++i)
                velocities[i] += 0.5 * dt * accelerations[i];

            for (size_t i = 0; i < bodies.size(); ++i)
                bodies[i].position += dt * velocities[i];

            BalancedFmmTree<d> tree_step(bodies, items_per_leaf, fault_tolerance_eps, soften_eps);
            for (size_t i = 0; i < bodies.size(); ++i)
                accelerations[i] = tree_step.evaluateForcefield(bodies[i].position) / bodies[i].q;

            for (size_t i = 0; i < bodies.size(); ++i)
                velocities[i] += 0.5 * dt * accelerations[i];
        }

        double t_end = omp_get_wtime();
        double total_time = t_end - t_start;
        double avg_time_per_step = total_time / n_steps;

        double total_ms_time = total_time * 1000;
        double avg_ms_time_per_step = avg_time_per_step * 1000;

        // Append results to the file
        std::ofstream out_eff("integration_efficiency.txt", std::ios::app);
        out_eff << N << " " << total_ms_time << " " << avg_ms_time_per_step << "\n";
        out_eff.close();

        std::cout << "N: " << N << ", total time (ms): " << total_ms_time
                  << ", avg time per step (ms): " << avg_ms_time_per_step << std::endl;
    }

    return 0;
}
