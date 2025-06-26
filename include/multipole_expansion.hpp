#ifndef MULTIPOLE_EXPANSION_H
#define MULTIPOLE_EXPANSION_H

#include <complex>
#include <type_traits>
#include <numeric>
#include <algorithm>
#include <stdexcept>

#include "series_expansion.hpp"

namespace fmm
{

    template <unsigned d>
    struct MultipoleExpansion : SeriesExpansion<d>
    {
    };

    /******************************************************************************/
    /*                      2D Multipole Expansion Implementation                 */
    /******************************************************************************/
    template <>
    struct MultipoleExpansion<2> : SeriesExpansion<2>
    {

        using Vector = Vector_<2>;
        using Body = Body_<2>;
        using Super = SeriesExpansion<2>;
        using ME = MultipoleExpansion<2>;

        MultipoleExpansion() : Super() {};
        MultipoleExpansion(const Vector &center, unsigned order,
                           std::vector<Body> &sources);
        MultipoleExpansion(const Vector &center, std::vector<const ME *> &expansions);

        std::vector<Complex> shift(const Complex &shift) const;

        double evaluatePotential(const Vector &eval_point) const;
        Vector evaluateForcefield(const Vector &eval_point) const;
    };

    MultipoleExpansion<2>::MultipoleExpansion(const Vector_<2> &center,
                                              unsigned order, std::vector<Body_<2>> &sources) : Super(center, order)
    {

        // Compute series expansion coefficients a_0 through a_order:
        for (std::size_t i = 0; i < sources.size(); ++i)
        {
            Body &src = sources[i];
            Complex z{src.position[0], src.position[1]};
            Complex z_rel = z - this->center; // Express z in box-local coordinates, // (z_i - z0)
            Complex z_rel_pow = z_rel;
            (*this)(0) += src.sourceStrength(); // a_0 = sum q_i
            for (unsigned j = 1; j <= order; ++j)
            {
                (*this)(j) -= src.sourceStrength() * z_rel_pow; // -q_i (z_i-z0)^j
                z_rel_pow *= z_rel;
            }
        }
        for (unsigned j = 1; j <= order; ++j)
        {
            (*this)(j) /= (double)j;
        }
    }

    // Construct expansion from other expansions by shifting
    MultipoleExpansion<2>::MultipoleExpansion(const Vector &center,
                                              std::vector<const ME *> &expansions) : Super(center, expansions.at(0)->order)
    {
        for (const ME *me : expansions)
        {
            Complex shift_vec = me->center - this->center;
            std::vector<Complex> shifted_coefficients = me->shift(shift_vec);
            std::transform(this->coefficients.begin(), this->coefficients.end(),
                           shifted_coefficients.begin(), this->coefficients.begin(),
                           std::plus<Complex>());
        }
    }

    // Shift is the vector (complex number) from the new to the old center.
    std::vector<Complex> MultipoleExpansion<2>::shift(const Complex &shift) const
    {
        std::vector<Complex> shifted_coefficients(this->order + 1);
        const typename tables::BinomialTable &binomial_table = Super::binomial_table;
        typename tables::template PowTable<Complex> shift_pow_table(shift, this->order);
        double Q = (*this)(0).real();
        shifted_coefficients[0] = Q;
        for (int l = 1; l <= this->order; ++l)
        {
            shifted_coefficients[l] = -Q * shift_pow_table(l) / (double)l;
            for (int k = 1; k <= l; ++k)
            {
                shifted_coefficients[l] += (*this)(k)*shift_pow_table(l - k) * binomial_table(l - 1, k - 1); // [(4.15), 1]
            }
        }
        return shifted_coefficients;
    }

    double MultipoleExpansion<2>::evaluatePotential(const Vector_<2> &eval_point) const
    {
        Complex z{eval_point[0], eval_point[1]}; // get complex repr.
        Complex z_rel = z - this->center;
        double Q = (*this)(0).real();
        Complex result = Q * log(z_rel);
        Complex z_rel_inv_pow = 1. / z_rel;
        for (unsigned j = 1; j < this->coefficients.size(); ++j)
        {
            result += (*this)(j)*z_rel_inv_pow;
            z_rel_inv_pow /= z_rel;
        }
        // Return +result.real() for the gravitational potential.
        return result.real();
    }

    Vector_<2> MultipoleExpansion<2>::evaluateForcefield(const Vector_<2> &eval_point) const
    {
        Complex z{eval_point[0], eval_point[1]}; // get complex repr.
        Complex z_rel = z - this->center;
        double Q = (*this)(0).real();
        Complex result = Q / z_rel;
        Complex z_rel_inv_pow = 1. / (z_rel * z_rel);
        for (unsigned j = 1; j < this->coefficients.size(); ++j)
        {
            result -= (double)j * (*this)(j)*z_rel_inv_pow;
            z_rel_inv_pow /= z_rel;
        }
        // The gravitational field is given by {{-result.real(), result.imag()}}.
        return {{-result.real(), result.imag()}};
    }

} // namespace fmm

#endif
