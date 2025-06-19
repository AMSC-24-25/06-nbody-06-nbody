// examples/integrator.cpp
#include "../balanced_fmm_tree.hpp"
#include <fstream>
#include <iomanip>
#include <random>
#include <cmath>
#include <vector>
#include <iostream>

// Namespace and type aliases
using namespace fmm;

constexpr size_t N = 1000;             // Number of particles
constexpr size_t items_per_leaf = 128; // Items per leaf in the FMM tree
constexpr size_t d = 2;
constexpr double extent = 10.0;   // Spatial extent of the galaxy
constexpr double dt = 0.0001;     // Time step
constexpr size_t n_steps = 10000; // Number of steps
constexpr double eps = 0.1;       // Softening parameter for force
const double G = 1.0;

using Vec = Vector_<d>;
using Src = Body_<d>;

// --- Minimal Particle struct ---
struct Particle
{
    Vec pos, vel, acc;
    double mass;

    Particle(const Vec &p, const Vec &v, double m)
        : pos(p), vel(v), acc{}, mass(m) {}
};

// --- Stable Ring with Spiral Arms Generator ---
std::vector<Particle> generateStableRing(
    int N, double extent, double total_mass, unsigned int seed = 12345)
{
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> unit_dist(0.0, 1.0);
    std::normal_distribution<double> normal_dist(0.0, 1.0);

    std::vector<Particle> particles;
    particles.reserve(N + 1);

    // Ring parameters - adjusted for stability
    const double central_mass = 0.6 * total_mass; // Much heavier central mass
    const double ring_mass = total_mass - central_mass;
    const double ring_radius = 0.4 * extent; // Larger ring radius
    const double ring_width = 0.2 * extent;  // Narrower ring for stability
    const double inner_hole = 0.2 * extent;  // Larger inner hole
    const int num_arms = 3;
    const double spiral_strength = 0.1; // Much weaker spiral pattern

    // Add central massive object (creates the hole and provides stability)
    Vec center_pos = Vec{{0.0, 0.0}};
    Vec center_vel = Vec{{0.0, 0.0}};
    particles.emplace_back(center_pos, center_vel, central_mass);

    // Generate ring particles with minimal spiral structure
    for (int i = 0; i < N; ++i)
    {
        // Start with base ring radius and small random variation
        double r_base = ring_radius + normal_dist(rng) * ring_width * 0.4;

        // Random angle for this particle
        double angle_base = 2.0 * M_PI * unit_dist(rng);

        // Very subtle spiral modulation
        double spiral_mod = 0.0;
        for (int arm = 0; arm < num_arms; ++arm)
        {
            double arm_angle = arm * 2.0 * M_PI / num_arms;
            double spiral_phase = 2.0 * (angle_base - arm_angle);
            spiral_mod += std::cos(spiral_phase) * std::exp(-std::pow(angle_base - arm_angle, 2) / 0.5);
        }

        // Apply very small spiral perturbation
        double r = r_base + spiral_strength * ring_width * spiral_mod;

        // Ensure we stay within safe bounds
        r = std::max(r, inner_hole + 0.1 * extent);
        r = std::min(r, ring_radius + 2.0 * ring_width);

        // Convert to Cartesian coordinates
        double x = r * std::cos(angle_base);
        double y = r * std::sin(angle_base);
        Vec pos = Vec{{x, y}};

        // Calculate circular velocity for this radius
        double actual_r = pos.norm();
        // Use central mass for velocity calculation
        double v_circular = std::sqrt(G * central_mass / actual_r);

        // Boost velocity to ensure stability - particles need to be slightly over-rotating
        v_circular *= 4; // 4 times faster than pure circular to prevent collapse

        // Tangential velocity (counter-clockwise)
        Vec tangential = Vec{{-pos[1] / actual_r, pos[0] / actual_r}};
        Vec vel = tangential * v_circular;

        // Add tiny random velocity (much smaller)
        vel[0] += normal_dist(rng) * 0.001 * v_circular;
        vel[1] += normal_dist(rng) * 0.001 * v_circular;

        double mass = ring_mass / N;
        particles.emplace_back(pos, vel, mass);
    }

    return particles;
}

// --- I/O helper ---
void write_positions(const std::vector<Particle> &parts, std::ofstream &out, int step)
{
    out << "# Timestep " << step << "\n";
    for (size_t i = 0; i < parts.size(); ++i)
    {
        out << i << "\t" << parts[i].pos[0] << "\t" << parts[i].pos[1] << "\n";
    }
    out << "\n";
}

// --- Energies ---
double total_kinetic(const std::vector<Particle> &particles)
{
    double E = 0;
    for (const auto &p : particles)
    {
        double v2 = p.vel.norm_sq();
        E += 0.5 * p.mass * v2;
    }
    return E;
}

double total_potential(const std::vector<Particle> &particles)
{
    // Use FMM potentials, but for energy, double-counting is avoided
    std::vector<Src> sources;
    for (const auto &p : particles)
    {
        sources.emplace_back(p.pos, p.mass);
    }
    auto V = fields::particlePotentialEnergies<d>(sources);
    double E = 0;
    for (size_t i = 0; i < particles.size(); ++i)
        E += 0.5 * V[i]; // half to avoid double counting
    return E;
}

int main()
{
    // --- Stable ring with spiral arms ---
    double total_mass = N * 1.0;
    std::vector<Particle> particles = generateStableRing(N, extent, total_mass, 42);

    std::ofstream out_positions("particle_positions.txt");
    std::ofstream out_energy("energy_vs_time.txt");
    out_energy << "t\tE_kin\tE_pot\tE_tot\n";
    write_positions(particles, out_positions, 0);

    // Initialize: calculate initial acceleration using FMM
    std::vector<Src> sources(particles.size());
    for (size_t i = 0; i < particles.size(); ++i)
        sources[i] = Src(particles[i].pos, particles[i].mass);
    BalancedFmmTree<d> tree(sources, items_per_leaf, eps);
    for (size_t i = 0; i < particles.size(); ++i)
        particles[i].acc = tree.evaluateForcefield(particles[i].pos) / particles[i].mass;

    for (size_t step = 1; step <= n_steps; ++step)
    {
        // --- Leapfrog step 1: v(t+dt/2) ---
        for (auto &p : particles)
            p.vel += 0.5 * dt * p.acc;

        // --- Step positions: x(t+dt) ---
        for (auto &p : particles)
            p.pos += dt * p.vel;

        // --- Build FMM for new positions, compute new acc ---
        for (size_t i = 0; i < particles.size(); ++i)
            sources[i] = Src(particles[i].pos, particles[i].mass);
        BalancedFmmTree<d> tree_step(sources, items_per_leaf, eps);
        for (size_t i = 0; i < particles.size(); ++i)
            particles[i].acc = tree_step.evaluateForcefield(particles[i].pos) / particles[i].mass;

        // --- Leapfrog step 2: v(t+dt) ---
        for (auto &p : particles)
            p.vel += 0.5 * dt * p.acc;

        // --- Output every X steps ---
        if (step % 100 == 0)
            write_positions(particles, out_positions, step);

        // --- Output energy ---
        if (step % 1 == 0)
        {
            double E_kin = total_kinetic(particles);
            double E_pot = total_potential(particles);
            out_energy << std::setprecision(8) << step * dt << "\t" << E_kin << "\t" << E_pot << "\t" << (E_kin + E_pot) << "\n";
        }
    }
    out_positions.close();
    out_energy.close();
    return 0;
}