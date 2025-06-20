#include "../balanced_fmm_tree.hpp"
#include "../particle_generators.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>
#include <fstream>

using namespace fmm;

// Here are some default parameters for the test
constexpr size_t N = 1000;
constexpr size_t items_per_leaf = 64;
constexpr size_t d = 2;
constexpr double extent = 10.0;
constexpr double dt = 0.001;
constexpr size_t n_steps = 1000; // Number of steps
constexpr double fault_tolerance_eps = 0.01;
constexpr double soften_eps = 0.01;
const double G = 1.0;

using Vec = Vector_<d>;
using Src = Body_<d>;

struct FmmError
{
    double abs_tot;
    double rel_tot;
    double abs_max;
    double rel_max;
};

// Function to write positions of bodies to a file
void write_positions(const std::vector<Src> &bodies, std::ofstream &out, int step)
{
    out << "# Timestep " << step << "\n";
    for (size_t i = 0; i < bodies.size(); ++i)
        out << i << "\t" << bodies[i].position[0] << "\t" << bodies[i].position[1] << "\n";
    out << "\n";
}

// Function to compute FMM error against direct potential calculation
FmmError compute_fmm_error(
    std::vector<Src> &bodies,
    size_t items_per_leaf,
    double fault_tolerance_eps,
    double soften_eps,
    const std::vector<double> &potential_direct)
{
    BalancedFmmTree<d> tree(bodies, items_per_leaf, fault_tolerance_eps, soften_eps);
    std::vector<double> potential_fmm(bodies.size());
    for (size_t i = 0; i < bodies.size(); ++i)
        potential_fmm[i] = tree.evaluatePotential(bodies[i].position);

    double sum_sq_error = 0, sum_sq_exact = 0;
    double max_abs_error = 0, max_rel_error = 0;
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        double diff = potential_direct[i] - potential_fmm[i];
        sum_sq_error += diff * diff;
        sum_sq_exact += potential_direct[i] * potential_direct[i];
        max_abs_error = std::max(max_abs_error, std::abs(diff));
        if (std::abs(potential_direct[i]) > 1e-12)
            max_rel_error = std::max(max_rel_error, std::abs(diff) / std::abs(potential_direct[i]));
    }
    double abs_tot = std::sqrt(sum_sq_error);
    double rel_tot = std::sqrt(sum_sq_error) / std::sqrt(sum_sq_exact);

    return {abs_tot, rel_tot, max_abs_error, max_rel_error};
}

// Function to sweep through different fault tolerance epsilons and compute errors
void sweep_fault_tolerance(
    std::vector<Src> &bodies,
    double soften_eps,
    size_t items_per_leaf,
    const std::vector<double> &fault_tolerance_list,
    const std::string &out_filename)
{
    std::ofstream out(out_filename);
    out << "fault_tolerance_eps\tE_abs_tot\tE_rel_tot\tE_abs_max\tE_rel_max\n";

    std::vector<double> potential_direct(bodies.size());
    for (size_t i = 0; i < bodies.size(); ++i)
        potential_direct[i] = fields::potential<d>(bodies, bodies[i].position);

    for (double eps : fault_tolerance_list)
    {
        auto error = compute_fmm_error(bodies, items_per_leaf, eps, soften_eps, potential_direct);
        out << std::setprecision(16) << std::fixed
            << eps << "\t" << error.abs_tot << "\t" << error.rel_tot << "\t"
            << error.abs_max << "\t" << error.rel_max << "\n";
    }
    out.close();
}

// Function to sweep through different numbers of items per leaf and compute errors
void sweep_items_per_leaf(
    std::vector<Src> &bodies,
    double soften_eps,
    double fault_tolerance_eps,
    const std::vector<size_t> &items_per_leaf_list,
    const std::string &out_filename)
{
    std::ofstream out(out_filename);
    out << "items_per_leaf\tE_abs_tot\tE_rel_tot\tE_abs_max\tE_rel_max\n";

    std::vector<double> potential_direct(bodies.size());
    for (size_t i = 0; i < bodies.size(); ++i)
        potential_direct[i] = fields::potential<d>(bodies, bodies[i].position);

    for (auto items_leaf : items_per_leaf_list)
    {
        auto error = compute_fmm_error(bodies, items_leaf, fault_tolerance_eps, soften_eps, potential_direct);
        out << std::setprecision(16) << std::fixed
            << items_leaf << "\t" << error.abs_tot << "\t" << error.rel_tot << "\t"
            << error.abs_max << "\t" << error.rel_max << "\n";
    }
    out.close();
}

// Function to test the integration over time and error accumulation
void integration_over_time_error_test()
{
    double total_mass = N * 1.0;
    std::vector<Src> bodies = generateUniformSquare<d>(N, extent, total_mass);

    // Initial velocities: random, zero net momentum
    std::vector<Vec> velocities(bodies.size());
    std::mt19937 rng(42);
    std::normal_distribution<double> vel_dist(0.0, 0.1);
    for (size_t i = 0; i < bodies.size(); ++i)
        for (size_t j = 0; j < d; ++j)
            velocities[i][j] = vel_dist(rng);
    Vec mean_v{0.0};
    for (const auto &v : velocities)
        mean_v = mean_v + v;
    for (size_t j = 0; j < d; ++j)
        mean_v[j] /= bodies.size();
    for (auto &v : velocities)
        v = v - mean_v;

    std::vector<Vec> accelerations(bodies.size());

    std::ofstream out_positions("accuracy_positions.txt");
    std::ofstream out_error("accuracy_error_vs_time.txt");
    out_error << "t\tE_abs_tot\tE_rel_tot\tE_abs_max\tE_rel_max\n";
    write_positions(bodies, out_positions, 0);

    // Initial acceleration
    BalancedFmmTree<d> tree(bodies, items_per_leaf, fault_tolerance_eps, soften_eps);
    for (size_t i = 0; i < bodies.size(); ++i)
        accelerations[i] = tree.evaluateForcefield(bodies[i].position) / bodies[i].q;

    for (size_t step = 1; step <= n_steps; ++step)
    {
        // Leapfrog: v(t+dt/2)
        for (size_t i = 0; i < bodies.size(); ++i)
            velocities[i] += 0.5 * dt * accelerations[i];

        // x(t+dt)
        for (size_t i = 0; i < bodies.size(); ++i)
            bodies[i].position += dt * velocities[i];

        // Update FMM and accelerations
        BalancedFmmTree<d> tree_step(bodies, items_per_leaf, fault_tolerance_eps, soften_eps);
        for (size_t i = 0; i < bodies.size(); ++i)
            accelerations[i] = tree_step.evaluateForcefield(bodies[i].position) / bodies[i].q;

        // Leapfrog: v(t+dt)
        for (size_t i = 0; i < bodies.size(); ++i)
            velocities[i] += 0.5 * dt * accelerations[i];

        // Output
        if (step % 10 == 0)
            write_positions(bodies, out_positions, step);

        // Evaluate FMM and direct potential error every 10 steps
        if (step % 10 == 0)
        {
            std::vector<double> potential_fmm(N, 0.0), potential_direct(N, 0.0);
            for (size_t i = 0; i < N; ++i)
                potential_fmm[i] = tree_step.evaluatePotential(bodies[i].position);
            for (size_t i = 0; i < N; ++i)
                potential_direct[i] = fields::potential<d>(bodies, bodies[i].position);

            double sum_sq_error = 0, sum_sq_exact = 0;
            double max_abs_error = 0, max_rel_error = 0;
            for (size_t i = 0; i < N; ++i)
            {
                double diff = potential_direct[i] - potential_fmm[i];
                sum_sq_error += diff * diff;
                sum_sq_exact += potential_direct[i] * potential_direct[i];
                max_abs_error = std::max(max_abs_error, std::abs(diff));
                if (std::abs(potential_direct[i]) > 1e-12)
                    max_rel_error = std::max(max_rel_error, std::abs(diff) / std::abs(potential_direct[i]));
            }
            double E_abs_tot = std::sqrt(sum_sq_error);
            double E_rel_tot = std::sqrt(sum_sq_error) / std::sqrt(sum_sq_exact);

            out_error << std::setprecision(16) << std::fixed
                      << step * dt << "\t" << E_abs_tot << "\t" << E_rel_tot << "\t"
                      << max_abs_error << "\t" << max_rel_error << "\n";
        }
    }
    out_positions.close();
    out_error.close();

    std::cout << "Integration test complete. Output written to accuracy_positions.txt and accuracy_error_vs_time.txt\n";
}

// Function to write a spatial error map of FMM vs direct potential
void write_spatial_error_map(
    std::vector<Src> &sources,
    size_t items_per_leaf,
    double fault_tolerance_eps,
    double soften_eps,
    double xmin, double xmax, double ymin, double ymax,
    int ngrid,
    const std::string &out_filename)
{
    std::ofstream out(out_filename);
    out << "# x y log10_rel_error\n";

    BalancedFmmTree<d> tree(sources, items_per_leaf, fault_tolerance_eps, soften_eps);

    // In our case the smaller grid size is about 0.05*0.05
    for (int ix = 0; ix < ngrid; ++ix)
    {
        double x = xmin + (xmax - xmin) * ix / (ngrid - 1);
        for (int iy = 0; iy < ngrid; ++iy)
        {
            double y = ymin + (ymax - ymin) * iy / (ngrid - 1);
            Vec eval_point({x, y});

            double fmm = tree.evaluatePotential(eval_point);
            double direct = fields::potential<d>(sources, eval_point);
            double rel_error = std::abs(fmm - direct) / (std::abs(direct) + 1e-14); // avoid 0-divide

            // Clamp to a minimum nonzero value before taking log10 to avoid -inf (same value)
            rel_error = std::max(rel_error, 1e-16);

            out << x << " " << y << " " << std::log10(rel_error) << "\n";
        }
        out << "\n";
    }
    out.close();
}

// Main function to run the tests
int main(int argc, char *argv[])
{
    std::string mode = "spatial_error_map"; // set mode here
    if (argc > 1)
        mode = argv[1];

    if (mode == "integration")
    {
        integration_over_time_error_test();
    }
    else if (mode == "sweep_fault_tolerance")
    {
        double total_mass = N * 1.0;
        std::vector<Src> bodies = generateUniformSquare<d>(N, extent, total_mass);

        std::vector<double> fault_tolerance_list = {1e-5, 1e-4, 1e-3, 1e-2, 1e-1, 1.0};
        sweep_fault_tolerance(bodies, soften_eps, items_per_leaf, fault_tolerance_list,
                              "fmm_accuracy_vs_fault_tolerance.txt");
        std::cout << "Fault tolerance sweep complete: see fmm_accuracy_vs_fault_tolerance.txt\n";
    }
    else if (mode == "sweep_items_per_leaf")
    {
        double total_mass = N * 1.0;
        std::vector<Src> bodies = generateUniformSquare<d>(N, extent, total_mass);

        std::vector<size_t> items_per_leaf_list = {4, 8, 16, 32, 64, 128, 256};
        sweep_items_per_leaf(bodies, soften_eps, fault_tolerance_eps, items_per_leaf_list,
                             "fmm_accuracy_vs_items_per_leaf.txt");
        std::cout << "Items per leaf sweep complete: see fmm_accuracy_vs_items_per_leaf.txt\n";
    }
    else if (mode == "spatial_error_map")
    {
        double total_mass = N * 1.0;
        std::vector<Src> bodies = generateUniformSquare<d>(N, extent, total_mass);
        double min_x = bodies[0].position[0], max_x = bodies[0].position[0];
        double min_y = bodies[0].position[1], max_y = bodies[0].position[1];
        for (const auto &b : bodies)
        {
            min_x = std::min(min_x, b.position[0]);
            max_x = std::max(max_x, b.position[0]);
            min_y = std::min(min_y, b.position[1]);
            max_y = std::max(max_y, b.position[1]);
        }
        // shrink by a small epsilon
        double margin = 0.05 * extent;
        min_x += margin;
        max_x -= margin;
        min_y += margin;
        max_y -= margin;
        write_spatial_error_map(
            bodies, items_per_leaf, fault_tolerance_eps, soften_eps,
            min_x, max_x, min_y, max_y, 200, "spatial_error_map.txt");
    }

    else
    {
        std::cerr << "Usage: " << argv[0] << " [integration|sweep_fault_tolerance|sweep_items_per_leaf]\n";
        return 1;
    }
    return 0;
}
