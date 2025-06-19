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

        struct AlmTable
        {

            int max_order = 0;
            std::vector<double> table;

            AlmTable(int order = 0)
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
                table.resize((2 * max_order + 1) * (2 * max_order + 1));

                for (int l = 0; l <= 2 * max_order; ++l)
                {
                    for (int m = -l; m <= l; ++m)
                    {
                        table[getTableIndex(l, m)] = A_coeff(l, m);
                    }
                }
            }

            unsigned getTableIndex(unsigned l, int m) const { return l * (l + 1) + m; }

            const double &operator()(unsigned l, int m) const
            {
                return table[getTableIndex(l, m)];
            }
        };

        struct SignTable
        {

            const std::function<double(int, int)> signFunction;
            int max_order = 0;
            std::vector<double> table;

            int offset;

            SignTable(const std::function<double(int, int)> signFunction,
                      int order = 0) : signFunction(signFunction)
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
                offset = 2 * max_order * (max_order + 1);
                table.resize((2 * max_order + 1) * (2 * max_order + 1));

                for (int l = -max_order; l <= max_order; ++l)
                {
                    for (int m = -max_order; m <= max_order; ++m)
                    {
                        table[getTableIndex(l, m)] = signFunction(l, m);
                    }
                }
            }

            unsigned getTableIndex(int l, int m) const
            {
                return offset + l * (2 * max_order + 1) + m;
            }

            const double &operator()(int l, int m) const
            {
                return table[getTableIndex(l, m)];
            }
        };

    } // namespace tables
} // namespace fmm

#endif