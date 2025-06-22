#ifndef ABSTRACT_FMM_TREE_H
#define ABSTRACT_FMM_TREE_H

#include <vector>
#include <tuple>
#include <numeric>
#include <stdexcept>
#include <cmath>
#include <cassert>
#include "vector.hpp"
#include "multipole_expansion.hpp"
#include "local_expansion.hpp"
#include "fmm_fields.hpp"

namespace fmm
{

    template <std::size_t d>
    class AbstractFmmTree
    {

    protected:
        using Vector = Vector_<d>;
        using Body = Body_<d>;
        using ME = MultipoleExpansion<d>;
        using LE = LocalExpansion<d>;

        const std::vector<Body> &sources;
        unsigned order;
        double eps;
        double force_smoothing_eps;

    public:
        AbstractFmmTree(std::vector<Body> &sources, double eps,
                        double force_smoothing_eps)
            : sources(sources), eps(eps), force_smoothing_eps(force_smoothing_eps)
        {
            static_assert(d == 2, "Only 2D FMM is implemented.");
            // Determine expansion order
            double A = std::accumulate(sources.begin(), sources.end(), 0.0,
                                       [](double acc, const Body &src)
                                       { return acc + std::abs(src.sourceStrength()); });
            this->order = (ceil(log(A / eps) / log(2)));
        };

        virtual double evaluatePotential(const Vector &eval_point) const = 0;
        virtual Vector evaluateForcefield(const Vector &eval_point) const = 0;
        std::vector<double> evaluateParticlePotentialEnergies() const;
        std::vector<Vector> evaluateParticleForces() const;

        unsigned getOrder() { return order; }
        double getAccuracyEps() { return eps; }

        virtual ~AbstractFmmTree() {}

    protected:
        std::tuple<Vector, Vector> getDataRange() const;

        template <typename FmmNode>
        void localToLocal(FmmNode &node);
        template <typename FmmNode>
        void multipoleToLocal(FmmNode &node);
    };

    template <std::size_t d>
    std::vector<double> AbstractFmmTree<d>::
        evaluateParticlePotentialEnergies() const
    {
        std::vector<double> potentials(sources.size());
#pragma omp parallel for schedule(dynamic)
        for (std::size_t i = 0; i < sources.size(); ++i)
        {
            potentials[i] = sources[i].sourceStrength() * evaluatePotential(sources[i].position);
        }
        return potentials;
    }

    template <std::size_t d>
    std::vector<Vector_<d>> AbstractFmmTree<d>::
        evaluateParticleForces() const
    {
        std::vector<Vector> forces(sources.size());
#pragma omp parallel for schedule(dynamic)
        for (std::size_t i = 0; i < sources.size(); ++i)
        {
            forces[i] = sources[i].sourceStrength() * evaluateForcefield(sources[i].position);
        }
        return forces;
    }

    template <std::size_t d>
    std::tuple<Vector_<d>, Vector_<d>> AbstractFmmTree<d>::getDataRange() const
    {
        Vector lower_bounds, upper_bounds;
        lower_bounds.fill(HUGE_VAL);
        upper_bounds.fill(-HUGE_VAL);

        std::for_each(sources.begin(), sources.end(), [&](Body p)
                      {
        Vector& pos = p.position;
        for(std::size_t i = 0; i < d; i++) {
            lower_bounds[i] = pos[i] < lower_bounds[i] ? pos[i] : lower_bounds[i];
            upper_bounds[i] = pos[i] > upper_bounds[i] ? pos[i] : upper_bounds[i];
        } });

        double paddingFactor = 1E-5;
        lower_bounds = lower_bounds + paddingFactor * lower_bounds;
        upper_bounds = upper_bounds + paddingFactor * upper_bounds;

        return std::make_tuple(lower_bounds, upper_bounds);
    }

    template <std::size_t d>
    template <typename FmmNode>
    void AbstractFmmTree<d>::localToLocal(FmmNode &node)
    {
        FmmNode *parent = static_cast<FmmNode *>(node.parent);
        assert(parent != nullptr);
        assert(parent->local_expansion.coefficients.size() > 0);
        node.local_expansion += LE(node.center, parent->local_expansion);
    }

    template <std::size_t d>
    template <typename FmmNode>
    void AbstractFmmTree<d>::multipoleToLocal(FmmNode &node)
    {
        std::vector<const ME *> incoming;
        for (FmmNode *interaction_partner : node.interaction_list)
        {
            incoming.push_back(&interaction_partner->multipole_expansion);
        }
        if (!incoming.empty())
        {
            node.local_expansion += LE(node.center, incoming);
        }
    }

} // namespace fmm

#endif