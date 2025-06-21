#include "../balanced_fmm_tree.hpp"
#include "../particle_generators.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include <algorithm> // for std::copy

using namespace fmm;

constexpr size_t d = 2;
constexpr double extent = 10.0;
constexpr double dt = 0.00001;
constexpr size_t n_steps = 100;
const double G = 1.0;
constexpr double soften_eps = 0.01;

enum SweepMode
{
    SWEEP_N,
    SWEEP_ITEMS_PER_LEAF,
    SWEEP_EPS
};

// --- Core function ---
void run_fmm_sim(
    size_t N, size_t items_per_leaf, double fault_tolerance_eps,
    const std::vector<Body_<d>>& init_bodies,
    const std::vector<Vector_<d>>& init_velocities,
    std::ofstream &out_eff)
{
    using Vec = Vector_<d>;
    using Src = Body_<d>;

    // Clone initial state for each run
    std::vector<Src> bodies = init_bodies;
    std::vector<Vec> velocities = init_velocities;
    std::vector<Vec> accelerations(bodies.size());

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

    out_eff << N << " " << items_per_leaf << " " << fault_tolerance_eps << " "
            << total_ms_time << " " << avg_ms_time_per_step << "\n";

    std::cout << "N: " << N
              << ", items_per_leaf: " << items_per_leaf
              << ", eps: " << fault_tolerance_eps
              << ", total time (ms): " << total_ms_time
              << ", avg time per step (ms): " << avg_ms_time_per_step << std::endl;
}

int main()
{
    SweepMode mode = SWEEP_N; 
    size_t default_N = 8192;
    size_t default_items_per_leaf = 64;
    double default_fault_tolerance_eps = 0.01;
    double total_mass;

    // Prepare output file (overwrite each run)
    std::ofstream out_eff("integration_efficiency.txt");
    if (mode == SWEEP_N)
        out_eff << "# mode: N\n";
    else if (mode == SWEEP_ITEMS_PER_LEAF)
        out_eff << "# mode: items_per_leaf\n";
    else if (mode == SWEEP_EPS)
        out_eff << "# mode: eps\n";
    out_eff << "# N items_per_leaf eps total_time_ms avg_time_per_step_ms\n";
    out_eff << std::setprecision(10);

    if (mode == SWEEP_N)
    {
        std::vector<size_t> N_list = {2000, 4000, 8000, 16000};
        for (size_t N : N_list)
        {
            // For each N, generate its own particles
            total_mass = N * 1.0;
            std::vector<Body_<d>> init_bodies = generateUniformSquare<d>(N, extent, total_mass, 42);
            std::vector<Vector_<d>> init_velocities(N, Vector_<d>{{0.0, 0.0}});
            run_fmm_sim(N, default_items_per_leaf, default_fault_tolerance_eps, init_bodies, init_velocities, out_eff);
        }
    }
    else if (mode == SWEEP_ITEMS_PER_LEAF)
    {
        // Generate initial state ONCE for this N!
        total_mass = default_N * 1.0;
        std::vector<Body_<d>> init_bodies = generateUniformSquare<d>(default_N, extent, total_mass, 42);
        std::vector<Vector_<d>> init_velocities(default_N, Vector_<d>{{0.0, 0.0}});
        std::vector<size_t> items_per_leaf_list = {16, 32, 64, 128, 256, 512};
        for (size_t items_per_leaf : items_per_leaf_list)
            run_fmm_sim(default_N, items_per_leaf, default_fault_tolerance_eps, init_bodies, init_velocities, out_eff);
    }
    else if (mode == SWEEP_EPS)
    {
        // Generate initial state ONCE for this N!
        total_mass = default_N * 1.0;
        std::vector<Body_<d>> init_bodies = generateUniformSquare<d>(default_N, extent, total_mass, 42);
        std::vector<Vector_<d>> init_velocities(default_N, Vector_<d>{{0.0, 0.0}});
        std::vector<double> eps_list = {1e-1, 1e-3, 1e-5, 1e-7, 1e-9};
        for (double eps : eps_list)
            run_fmm_sim(default_N, default_items_per_leaf, eps, init_bodies, init_velocities, out_eff);
    }

    out_eff.close();
    return 0;
}
