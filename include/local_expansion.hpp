#ifndef LOCAL_EXPANSION_H
#define LOCAL_EXPANSION_H

#include "series_expansion.hpp"
#include "multipole_expansion.hpp"

namespace fmm
{

    template <std::size_t d>
    struct LocalExpansion : SeriesExpansion<d>
    {
    };

    /******************************************************************************/
    /*                      2D Local Expansion Implementation                     */
    /******************************************************************************/
    template <>
    struct LocalExpansion<2> : SeriesExpansion<2>
    {

        using Vector = Vector_<2>;
        using Body = Body_<2>;
        using Super = SeriesExpansion<2>;
        using ME = MultipoleExpansion<2>;
        using LE = LocalExpansion;

        LocalExpansion() : Super() {} // Empty default constructor
        LocalExpansion(const Vector &center, std::size_t order) : Super(center, order) {}
        LocalExpansion(const Vector &center, std::size_t order, std::vector<Body> &sources);
        LocalExpansion(const Vector &center, std::vector<const ME *> expansions);
        LocalExpansion(const Vector &center, const ME &incoming);
        LocalExpansion(const Vector &center, const LE &incoming);

        std::vector<Complex> multipoleToLocal(const ME &incoming) const;
        std::vector<Complex> shift(const Complex shift) const;

        double evaluatePotential(const Vector &eval_point) const;
        Vector evaluateForcefield(const Vector &eval_point) const;
    };

    // Implementation

    LocalExpansion<2>::LocalExpansion(const Vector_<2> &center, std::size_t order, std::vector<Body_<2>> &sources)
        : Super(center, order)
    {
        for (std::size_t i = 0; i < sources.size(); ++i)
        {
            Body &src = sources[i];
            Complex z{src.position[0], src.position[1]};
            Complex z_rel = z - this->center;
            Complex z_rel_inv_pow = 1. / z_rel;
            this->coefficients[0] += src.sourceStrength() * std::log(-z_rel);
            for (std::size_t j = 1; j <= order; ++j)
            {
                this->coefficients[j] -= src.sourceStrength() * z_rel_inv_pow;
                z_rel_inv_pow /= z_rel;
            }
        }
        for (std::size_t j = 1; j <= order; ++j)
        {
            this->coefficients[j] /= (double)j;
        }
    }

    LocalExpansion<2>::LocalExpansion(const Vector_<2> &center, std::vector<const ME *> expansions)
        : Super(center, expansions.at(0)->order)
    {
        for (const ME *me : expansions)
        {
            *this += LocalExpansion(center, *me);
        }
    }

    LocalExpansion<2>::LocalExpansion(const Vector_<2> &center, const ME &incoming)
        : Super(center, incoming.order)
    {
        assert(incoming.order > 0);
        this->coefficients = multipoleToLocal(incoming);
    }

    LocalExpansion<2>::LocalExpansion(const Vector_<2> &center, const LocalExpansion &incoming)
        : Super(center, incoming.order)
    {
        assert(incoming.order > 0);
        Complex shift_vec = incoming.center - this->center;
        this->coefficients = incoming.shift(shift_vec);
    }

    std::vector<Complex> LocalExpansion<2>::multipoleToLocal(const ME &incoming) const
    {
        Complex z0 = incoming.center - this->center;
        std::vector<Complex> coefficients(this->order + 1);

        const tables::BinomialTable &binomial_table = Super::binomial_table;
        const tables::PowTable<Complex> z0_inv_pow_table(1. / z0, this->order);

        coefficients[0] = incoming(0) * std::log(-z0);
        for (int k = 1; k <= this->order; ++k)
        {
            double sign = k % 2 == 0 ? 1 : -1;
            coefficients[0] += sign * incoming(k) * z0_inv_pow_table(k);
        }
        for (int l = 1; l <= this->order; ++l)
        {
            Complex b_l = -incoming(0) / (double)l;
            for (int k = 1; k < this->order; ++k)
            {
                double sign = k % 2 == 0 ? 1 : -1;
                b_l += sign * incoming(k) * z0_inv_pow_table(k) * binomial_table(l + k - 1, k - 1);
            }
            b_l *= z0_inv_pow_table(l);
            coefficients[l] = b_l;
        }
        return coefficients;
    }

    std::vector<Complex> LocalExpansion<2>::shift(const Complex shift) const
    {
        std::vector<Complex> shifted_coefficients{this->coefficients};
        for (int j = 0; j < this->order; ++j)
        {
            for (int k = this->order - j - 1; k < this->order; ++k)
            {
                shifted_coefficients[k] -= shift * shifted_coefficients[k + 1];
            }
        }
        return shifted_coefficients;
    }

    double LocalExpansion<2>::evaluatePotential(const Vector_<2> &eval_point) const
    {
        Complex z{eval_point[0], eval_point[1]};
        const Complex z_rel = z - this->center;
        Complex result{};
        Complex z_rel_pow = 1;
        for (std::size_t k = 0; k < this->coefficients.size(); ++k)
        {
            result += (*this)(k)*z_rel_pow;
            z_rel_pow *= z_rel;
        }
        return result.real();
    }

    Vector_<2> LocalExpansion<2>::evaluateForcefield(const Vector_<2> &eval_point) const
    {
        Complex z{eval_point[0], eval_point[1]};
        Complex z_rel = z - this->center;
        Complex result{};
        Complex z_rel_pow = 1;
        for (std::size_t k = 1; k < this->coefficients.size(); ++k)
        {
            result += (double)k * (*this)(k)*z_rel_pow;
            z_rel_pow *= z_rel;
        }
        return {{-result.real(), result.imag()}};
    }

} // namespace fmm

#endif
