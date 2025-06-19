#ifndef FMM_FIELDS_H
#define FMM_FIELDS_H

#include <vector>
#include <cmath>
#include "vector.hpp"

namespace fmm
{
    namespace fields
    {

        // Gravitational (if grav = true) or Coulomb (if grav = false) potential
        // by a single source in 2D. With safe = true, this function checks
        // whether the evaluation point coincides with the source location
        // and returns zero in that case
        template <std::size_t d, bool grav = true, bool safe = true>
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
            double pot = src.q * std::log(r);
            if constexpr (grav)
            {
                return pot;
            }
            else
            {
                return -pot;
            }
        }

        // Overload for multiple sources
        template <std::size_t d, bool grav = true, bool safe = true>
        double potential(const std::vector<Body_<d>> &sources, const Vector_<d> &eval_point)
        {
            double pot = 0;
            for (const auto &s : sources)
                pot += potential<d, grav, safe>(s, eval_point);
            return pot;
        }

        // Gravitational (if grav = true) or Coulomb (if grav = false) force field
        // by a single source in 2D. With safe = true, this function checks
        // whether the evaluation point coincides with the source location and
        // returns the zero vector in that case
        template <std::size_t d, bool grav = true, bool safe = true>
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
            diff *= src.q / (r_sq + eps);
            if constexpr (grav)
            {
                return diff;
            }
            else
            {
                return -diff;
            }
        }

        // Overload for multiple sources
        template <std::size_t d, bool grav = true, bool safe = true>
        Vector_<d> forcefield(const std::vector<Body_<d>> &sources, const Vector_<d> &eval_point, const double eps = 0)
        {
            Vector_<d> frc{};
            for (const auto &s : sources)
                frc += forcefield<d, grav, safe>(s, eval_point, eps);
            return frc;
        }

        // Parallelized function for computation of all potentials on all particles
        template <std::size_t d, bool grav = true>
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
                    double temp_epot = Qi * potential<d, grav, false>(sources[j], eval_point);
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
        template <std::size_t d, bool grav = true>
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
                    Vector_<d> temp_force = Qi * forcefield<d, grav, false>(sources[j], eval_point, eps);
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
