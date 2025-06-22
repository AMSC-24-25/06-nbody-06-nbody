#ifndef FMM_FIELDS_H
#define FMM_FIELDS_H

#include <vector>
#include <cmath>
#include "vector.hpp"

namespace fmm
{
    namespace fields
    {

        // Gravitational potential by a single source in 2D.
        // With safe = true, this function checks whether the evaluation point
        // coincides with the source location and returns zero in that case
        template <std::size_t d, bool safe = true>
        double potential(const Body_<d> &src, const Vector_<d> &eval_point)
        {
            static_assert(d == 2, "Only 2D implementation is provided.");
            double r = (src.position - eval_point).norm();
            if constexpr (safe)
            {
                if (r == 0)
                {
                    return 0;
                }
            }
            // About the sign, here we just set the reference length L to 1 which means
            // that the potential is negative if r < 1 and positive if r > 1.
            // This is a common choice in physics, where the potential is defined
            // with respect to a reference point at infinity.
            // In 2D, the potential is logarithmic, so we use the formula:
            // G * M * ln(r), where M is the source strength (mass or charge
            // Actually the reference length is just a shift in the potential,
            // the shape of the potential is preserved.
            return src.q * std::log(r);
        }

        // Overload for multiple sources
        template <std::size_t d, bool safe = true>
        double potential(const std::vector<Body_<d>> &sources, const Vector_<d> &eval_point)
        {
            double pot = 0;
            for (const auto &s : sources)
                pot += potential<d, safe>(s, eval_point);
            return pot;
        }

        // Gravitational force field by a single source in 2D.
        // With safe = true, this function checks whether the evaluation point
        // coincides with the source location and returns the zero vector in that case
        template <std::size_t d, bool safe = true>
        Vector_<d> forcefield(const Body_<d> &src, const Vector_<d> &eval_point, const double eps = 0)
        {
            static_assert(d == 2, "Only 2D implementation is provided.");
            Vector_<d> diff = src.position - eval_point;
            double r_sq = diff.norm_sq();
            if constexpr (safe)
            {
                if (r_sq == 0)
                {
                    return Vector_<d>{};
                }
            }
            // Avoid division by zero by adding a small epsilon
            return diff * (src.q / (r_sq + eps));
        }

        // Overload for multiple sources
        template <std::size_t d, bool safe = true>
        Vector_<d> forcefield(const std::vector<Body_<d>> &sources, const Vector_<d> &eval_point, const double eps = 0)
        {
            Vector_<d> frc{};
            for (const auto &s : sources)
                frc += forcefield<d, safe>(s, eval_point, eps);
            return frc;
        }

        // Parallelized function for computation of all potentials on all particles
        template <std::size_t d>
        std::vector<double> particlePotentialEnergies(const std::vector<Body_<d>> &sources)
        {
            static_assert(d == 2, "Only 2D implementation is provided.");
            const std::size_t N = sources.size();
            std::vector<double> particle_potentials(N);
#pragma omp parallel for schedule(guided)
            for (std::size_t i = 0; i < N; ++i)
            {
                Vector_<d> eval_point = sources[i].position;
                double Qi = sources[i].sourceStrength();
                double particle_epot = 0;
                for (std::size_t j = 0; j < i; ++j)
                {
                    double temp_epot = Qi * potential<d, false>(sources[j], eval_point);
                    particle_epot += temp_epot;
#pragma omp atomic
                    particle_potentials[j] += temp_epot;
                }
#pragma omp atomic
                particle_potentials[i] += particle_epot;
            }
            return particle_potentials;
        }

        // Parallelized function for computation of all forces on all particles
        template <std::size_t d>
        std::vector<Vector_<d>> particleForces(const std::vector<Body_<d>> &sources, const double eps = 0)
        {
            static_assert(d == 2, "Only 2D implementation is provided.");
            const std::size_t N = sources.size();
            std::vector<Vector_<d>> particle_forces(N);
#pragma omp parallel for schedule(guided)
            for (std::size_t i = 0; i < N; ++i)
            {
                Vector_<d> eval_point = sources[i].position;
                double Qi = sources[i].sourceStrength();
                Vector_<d> particle_force{0}; // Force on part. i by all j < i
                for (std::size_t j = 0; j < i; ++j)
                {
                    Vector_<d> temp_force = Qi * forcefield<d, false>(sources[j], eval_point, eps);
                    particle_force += temp_force;
                    for (unsigned short k = 0; k < d; ++k)
                    {
#pragma omp atomic
                        particle_forces[j][k] -= temp_force[k];
                    }
                }
                for (unsigned short k = 0; k < d; ++k)
                {
#pragma omp atomic
                    particle_forces[i][k] += particle_force[k];
                }
            }
            return particle_forces;
        }

    } // namespace fields
} // namespace fmm

#endif