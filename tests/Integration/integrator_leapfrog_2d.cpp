#include "../../include/balanced_fmm_tree.hpp"
#include "../../include/particle_generators.hpp"
#include <fstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <cmath>

// Namespace and type aliases
using namespace fmm;

constexpr size_t N = 1000;
constexpr size_t items_per_leaf = 128;
constexpr size_t d = 2;
constexpr double extent = 10.0;
constexpr double dt = 0.0001;
constexpr size_t n_steps = 10000;
constexpr double fault_tolerance_eps = 0.1;
constexpr double soften_eps = 0.01; // Softening parameter for force
const double G = 1.0;

using Vec = Vector_<d>;
using Src = Body_<d>;

void write_positions(const std::vector<Src> &bodies, std::ofstream &out, int step)
{
    out << "# Timestep " << step << "\n";
    for (size_t i = 0; i < bodies.size(); ++i)
        out << i << "\t" << bodies[i].position[0] << "\t" << bodies[i].position[1] << "\n";
    out << "\n";
}

double total_kinetic(const std::vector<Vec> &velocities, const std::vector<Src> &bodies)
{
    double E = 0;
    for (size_t i = 0; i < bodies.size(); ++i)
        E += 0.5 * bodies[i].q * velocities[i].norm_sq();
    return E;
}

double total_potential(const std::vector<Src> &bodies)
{
    auto V = fields::particlePotentialEnergies<d>(bodies);
    double E = 0;
    for (size_t i = 0; i < bodies.size(); ++i)
        E += 0.5 * V[i];
    return E;
}

int main()
{
    // Generate bodies (positions + masses)
    double total_mass = N * 1.0;
    std::vector<Src> bodies = generateUniformSquare<d>(N, extent, total_mass, 42);

    // For velocities and accelerations, keep a parallel array (0th element = central mass)
    std::vector<Vec> velocities(bodies.size());
    std::vector<Vec> accelerations(bodies.size());

    // Initial velocities: only set for ring, central mass is 0 velocity
    double v_circular = std::sqrt(G * bodies[0].q);
    for (size_t i = 1; i < bodies.size(); ++i) // skip 0: central body
    {
        auto pos = bodies[i].position;
        double r = pos.norm();
        velocities[i][0] = -pos[1] / r * v_circular;
        velocities[i][1] = pos[0] / r * v_circular;
    }

    std::ofstream out_positions("particle_positions.txt");
    std::ofstream out_energy("energy_vs_time.txt");
    out_energy << "t\tE_kin\tE_pot\tE_tot\n";
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
        if (step % 100 == 0)
            write_positions(bodies, out_positions, step);

        if (step % 1 == 0)
        {
            double E_kin = total_kinetic(velocities, bodies);
            double E_pot = total_potential(bodies);
            out_energy << std::setprecision(8) << step * dt << "\t" << E_kin << "\t" << E_pot << "\t" << (E_kin + E_pot) << "\n";
        }
    }
    out_positions.close();
    out_energy.close();

}
