#ifndef SERIES_EXPANSION_H
#define SERIES_EXPANSION_H

#include <stdexcept>
#include <vector>
#include <complex>
#include <algorithm>
#include <iostream>

#include "fmm_tables.hpp"

namespace fmm
{

    template <std::size_t d>
    struct SeriesExpansion
    {
        static_assert(d == 2, "This implementation supports only 2D. 3D not implemented yet.");
    };

    /*----------------------------------------------------------------------------*/
    /*                      2D Series Expansion Implementation                    */
    /*----------------------------------------------------------------------------*/
    template <>
    struct SeriesExpansion<2>
    {

        int order;                   // Order of the expansion
        std::complex<double> center; // Center of the expansion (complex)
        std::vector<std::complex<double>> coefficients;
        static tables::BinomialTable binomial_table;

        SeriesExpansion() : order(), center(), coefficients() {}
        SeriesExpansion(const Vector_<2> &center, int order);

        SeriesExpansion &operator+=(const SeriesExpansion &rhs);
        std::complex<double> &operator()(unsigned n); // Access coefficient a_n
        const std::complex<double> &operator()(unsigned n) const;

        virtual double evaluatePotential(const Vector_<2> &eval_point) const = 0;
        virtual Vector_<2> evaluateForcefield(const Vector_<2> &eval_point) const = 0;

        friend std::ostream &operator<<(std::ostream &o, const SeriesExpansion &me);

        virtual ~SeriesExpansion() {}
    };

    tables::BinomialTable SeriesExpansion<2>::binomial_table;

    SeriesExpansion<2>::SeriesExpansion(const Vector_<2> &center, int order)
        : order(order), center({center[0], center[1]}), coefficients(order + 1)
    {
        if (order > binomial_table.max_order)
        {
            binomial_table.refresh(order);
        }
    }

    SeriesExpansion<2> &SeriesExpansion<2>::operator+=(const SeriesExpansion &rhs)
    {
        if (coefficients.size() != rhs.coefficients.size() || this->center != rhs.center)
        {
            throw std::logic_error("Series Expansions can only be added if they are of the same order and w.r.t. the same origin.");
        }
        std::transform(
            coefficients.begin(), coefficients.end(), rhs.coefficients.begin(),
            coefficients.begin(), std::plus<std::complex<double>>());
        return *this;
    }

    std::complex<double> &SeriesExpansion<2>::operator()(unsigned n)
    {
        return coefficients[n];
    }

    const std::complex<double> &SeriesExpansion<2>::operator()(unsigned n) const
    {
        return coefficients[n];
    }

    std::ostream &operator<<(std::ostream &o, const SeriesExpansion<2> &se)
    {
        o << "n\tRe a_n\tIm a_n\n";
        for (int n = 0; n <= se.order; ++n)
        {
            o << n << "\t" << se.coefficients[n].real() << "\t"
              << se.coefficients[n].imag() << "\n";
        }
        o << "\n";
        return o;
    }

} // namespace fmm

#endif
