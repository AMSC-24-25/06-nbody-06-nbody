#pragma once

#include "vector.hpp"
#include <random>
#include <vector>
#include <cmath>

namespace fmm
{

    template <std::size_t d>
    std::vector<Body_<d>> generateStableRing(
        int N, double extent, double total_mass, unsigned int seed = 12345,
        double G = 1.0)
    {
        using Vec = Vector_<d>;
        std::mt19937_64 rng(seed);
        std::uniform_real_distribution<double> unit_dist(0.0, 1.0);
        std::normal_distribution<double> normal_dist(0.0, 1.0);

        std::vector<Body_<d>> bodies;
        bodies.reserve(N + 1);

        // Central mass
        Vec center_pos{};
        bodies.emplace_back(center_pos, 0.6 * total_mass);

        const double ring_mass = total_mass - 0.6 * total_mass;
        const double ring_radius = 0.4 * extent;
        const double ring_width = 0.2 * extent;
        const double inner_hole = 0.2 * extent;
        const int num_arms = 3;
        const double spiral_strength = 0.1;

        for (int i = 0; i < N; ++i)
        {
            double r_base = ring_radius + normal_dist(rng) * ring_width * 0.4;
            double angle_base = 2.0 * M_PI * unit_dist(rng);

            // Spiral perturbation
            double spiral_mod = 0.0;
            for (int arm = 0; arm < num_arms; ++arm)
            {
                double arm_angle = arm * 2.0 * M_PI / num_arms;
                double spiral_phase = 2.0 * (angle_base - arm_angle);
                spiral_mod += std::cos(spiral_phase) * std::exp(-std::pow(angle_base - arm_angle, 2) / 0.5);
            }
            double r = r_base + spiral_strength * ring_width * spiral_mod;
            r = std::max(r, inner_hole + 0.1 * extent);
            r = std::min(r, ring_radius + 2.0 * ring_width);

            // Use Vector_ for position
            Vec pos{{r * std::cos(angle_base), r * std::sin(angle_base)}};
            double mass = ring_mass / N;

            bodies.emplace_back(pos, mass);
        }
        return bodies;
    }

    template <std::size_t d>
    std::vector<Body_<d>> generateUniformSquare(
        int N, double extent, double total_mass, unsigned int seed = 12345)
    {
        using Vec = Vector_<d>;
        std::mt19937_64 rng(seed);
        std::uniform_real_distribution<double> dist(-extent / 2.0, extent / 2.0);

        std::vector<Body_<d>> bodies;
        bodies.reserve(N);

        double mass = total_mass / N;
        for (int i = 0; i < N; ++i)
        {
            Vec pos{};
            for (std::size_t j = 0; j < d; ++j)
                pos[j] = dist(rng);
            bodies.emplace_back(pos, mass);
        }
        return bodies;
    }

} // namespace fmm
