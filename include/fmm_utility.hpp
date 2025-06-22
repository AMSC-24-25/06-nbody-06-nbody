#ifndef FMM_UTILITY_H
#define FMM_UTILITY_H

#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include <complex>

#include <gsl/gsl_sf_gamma.h>

#include "vector.hpp"

namespace fmm
{

    typedef std::complex<double> Complex;

    // Implements the function A_n^m := (-1)^n / sqrt((n-m)! (n+m)!) [(5.23), 1]
    double A_coeff(const int n, const int m)
    {

        if (n < 0)
        {
            throw std::logic_error("Argument n in A_coeff is expected "
                                   "to be nonnegative.");
        }

        return (n % 2 ? -1 : 1) / std::sqrt(gsl_sf_fact(n - m) * gsl_sf_fact(n + m));
    }

} // namespace fmm
#endif
