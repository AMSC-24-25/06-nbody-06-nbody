#pragma once
#include "Vector.hpp"
#include "Body.hpp"
#include <random>
#include <vector>
#include <cmath>

namespace nbody
{

    template <typename T>
    struct BodySystem
    {
        std::vector<Body<T, 2>> bodies;
        std::vector<Vector<T, 2>> velocities;
        std::vector<Vector<T, 2>> accelerations;

        BodySystem(size_t size) : bodies(), velocities(size), accelerations(size)
        {
            bodies.reserve(size);
        }
    };

    template <typename T>
    BodySystem<T> generateStableRing(
        int N, T extent, T total_mass, unsigned int seed = 12345, T G = 1.0)
    {
        using Vec = Vector<T, 2>;
        std::mt19937_64 rng(seed);
        std::uniform_real_distribution<T> unit_dist(0.0, 1.0);
        std::normal_distribution<T> normal_dist(0.0, 1.0);

        BodySystem<T> system(N + 1);

        // Central mass
        Vec center_pos({0.0, 0.0});
        system.bodies.emplace_back(0.6 * total_mass, center_pos, Vec({0.0, 0.0}));

        const T ring_mass = total_mass - 0.6 * total_mass;
        const T ring_radius = 0.4 * extent;
        const T ring_width = 0.4 * extent;  // Reduced width for more circular ring
        const T inner_hole = 0.2 * extent; // Smaller inner hole

        for (int i = 0; i < N; ++i)
        {
            // Distribute angles evenly with small random perturbation
            T angle_base = 2.0 * M_PI * i / N + normal_dist(rng) * 0.1;

            // Radial position with small random variation
            T r = ring_radius + normal_dist(rng) * ring_width * 0.3;

            // Ensure minimum distance from center
            r = std::max(r, inner_hole + 0.05 * extent);
            // Ensure maximum distance
            r = std::min(r, ring_radius + ring_width);

            Vec pos({r * std::cos(angle_base), r * std::sin(angle_base)});
            T mass = ring_mass / N;

            system.bodies.emplace_back(mass, pos, Vec({0.0, 0.0})); // velocity set later
        }

        // Set initial velocities: central mass has zero velocity
        system.velocities[0] = Vec({0.0, 0.0});
        system.accelerations[0] = Vec({0.0, 0.0});

        T v_circular = std::sqrt(G * system.bodies[0].getMass());
        for (size_t i = 1; i < system.bodies.size(); ++i)
        { // skip 0: central body
            auto pos = system.bodies[i].getPosition();
            T r = pos.norm();
            T v_scale = v_circular / std::sqrt(r);

            system.velocities[i] = Vec({-pos[1] / r * v_scale, pos[0] / r * v_scale});
            system.accelerations[i] = Vec({0.0, 0.0});

            // Update body velocity
            system.bodies[i] = Body<T, 2>(system.bodies[i].getMass(), pos, system.velocities[i]);
        }

        return system;
    }

} // namespace nbody