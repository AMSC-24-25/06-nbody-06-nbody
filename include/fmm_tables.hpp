#ifndef FMM_TABLES_H
#define FMM_TABLES_H

#include <vector>
#include <complex>
#include <cmath>

#include <gsl/gsl_sf_gamma.h>

#include "fmm_utility.hpp"

namespace fmm
{
    namespace tables
    {

        template <typename T>
        struct PowTable
        {

            const T &x;           // base
            const unsigned nmax;  // max. exponent
            std::vector<T> table; // stores [1, x, ... x^nmax]

            PowTable(const T &x, unsigned nmax) : x(x), nmax(nmax), table(nmax + 1)
            {

                T x_pow = 1;
                for (unsigned n = 0; n <= nmax; ++n)
                {
                    table[n] = x_pow;
                    x_pow *= x;
                }
            }

            const T &operator()(unsigned n) const { return table[n]; }
        };

        struct BinomialTable
        {

            int max_order = 0;
            std::vector<double> table;

            BinomialTable(int order = 0)
            {
                refresh(order);
                max_order = order;
            }

            void refresh(int order)
            {

                if (order <= max_order)
                {
                    return;
                }

                max_order = order;
                table.resize(((2 * max_order + 1) * (2 * max_order + 2)) / 2);

                for (int n = 0; n <= max_order; ++n)
                {
                    for (int k = 0; k <= n; ++k)
                    {
                        table[getTableIndex(n, k)] = gsl_sf_choose(n, k);
                    }
                }
            }

            unsigned getTableIndex(unsigned n, unsigned k) const
            {
                return (n * (n + 1)) / 2 + k;
            }

            const double &operator()(int n, int k) const
            {
                return table[getTableIndex(n, k)];
            }
        };

    } // namespace tables
} // namespace fmm

#endif