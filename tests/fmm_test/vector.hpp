#ifndef VECTOR_H
#define VECTOR_H

#include <array>
#include <iostream>
#include <numeric>
#include <cassert>
#include <cmath>

namespace fmm
{

    template <std::size_t d>
    struct Vector_
    {

        std::array<double, d> coords;

        Vector_(std::array<double, d> coords = {}) : coords(coords) {}
        Vector_(double fill_value) : coords() { this->fill(fill_value); }

        double &operator[](std::size_t index) { return coords[index]; }
        const double &operator[](std::size_t index) const { return coords[index]; }

        Vector_ operator+(const Vector_ &rhs) const
        {
            std::array<double, d> summed_coords;
            for (std::size_t i = 0; i < rhs.coords.size(); ++i)
            {
                summed_coords[i] = (*this)[i] + rhs[i];
            }
            return Vector_{summed_coords};
        }

        Vector_ operator*(const double s) const
        {
            std::array<double, d> scaled_coords = this->coords;
            for (double &c : scaled_coords)
            {
                c *= s;
            }
            return Vector_{scaled_coords};
        }

        Vector_ operator-() const { return -1. * *this; }
        Vector_ operator-(const Vector_ &rhs) const { return *this + (-rhs); }
        Vector_ &operator*=(const double s) { return *this = *this * s; }
        Vector_ &operator/=(const double s) { return *this *= (1 / s); }
        Vector_ &operator+=(const Vector_ &rhs) { return *this = *this + rhs; }
        Vector_ &operator-=(const Vector_ &rhs) { return *this = *this - rhs; }

        friend Vector_ operator*(const double s, const Vector_ &rhs) { return rhs * s; }
        friend std::ostream &operator<<(std::ostream &o, const Vector_ &p)
        {
            for (std::size_t i = 0; i < d - 1; i++)
            {
                o << p[i] << ", ";
            }
            o << p[d - 1];
            return o;
        }

        bool operator==(const Vector_ &rhs) const { return this->coords == rhs.coords; }
        bool operator!=(const Vector_ &rhs) const { return !(*this == rhs); }

        std::array<double, d> &data() { return coords; }
        const std::array<double, d> &data() const { return coords; }
        void fill(double val) { coords.fill(val); }

        double dot(const Vector_ &rhs) const
        {
            return std::inner_product(coords.begin(), coords.end(),
                                      rhs.data().begin(), 0.);
        }

        double norm() const
        {
            if constexpr (d == 1)
            {
                return std::abs(coords[0]);
            }
            else if (d == 2)
            {
                return std::hypot(coords[0], coords[1]);
            }

            return std::sqrt(this->dot(*this));
        }

        double norm_sq() const
        {
            if constexpr (d == 1)
            {
                return coords[0] * coords[0];
            }
            else if (d == 2)
            {
                return coords[0] * coords[0] + coords[1] * coords[1];
            }

            return this->dot(*this);
        }

        // Returns vector of (r, phi) of polar coordinates from vector (x, y) of
        // cartesian coordinates if d == 2, and is not implemented for other cases.
        Vector_ toSpherical() const
        {
            if constexpr (d == 2)
            {
                double r = this->norm();
                double phi = std::atan2(coords[1], coords[0]);

                return Vector_{{r, phi}};
            }
            else
            {
                throw std::logic_error("toSpherical() implemented only "
                                       "for 2 dimensions.");
            }
        }

        // Returns vector of (x, y) cartesian coordinates from vector (r, phi) of
        // polar coordinates if d == 2, and is not implemented for other cases.
        Vector_ toCartesian() const
        {
            if constexpr (d == 2)
            {
                double x = coords[0] * std::cos(coords[1]);
                double y = coords[0] * std::sin(coords[1]);

                return Vector_{{x, y}};
            }
            else
            {
                throw std::logic_error("toCartesian() implemented only "
                                       "for 2 dimensions.");
            }
        }
    };

    template <std::size_t d>
    struct Body_
    {

        Vector_<d> position;
        double q;

        Body_(Vector_<d> position = {}, double q = 0) : position(position), q(q) {}
        Body_(std::array<double, d> position, double q) : position(position), q(q) {}

        double &operator[](std::size_t index) { return position[index]; }
        const double &operator[](std::size_t index) const { return position[index]; }

        bool operator==(const Body_ &rhs) const
        {
            return this->q == rhs.q && this->position == rhs.position;
        }

        double sourceStrength() const { return q; }

        friend std::ostream &operator<<(std::ostream &o, const Body_ &p)
        {
            o << p.position << ", q = " << p.q;
            return o;
        }
    };

} // namespace fmm

namespace fmm
{
    template <std::size_t d>
    inline Vector_<d> operator/(const Vector_<d> &v, double s)
    {
        Vector_<d> result = v;
        for (auto &x : result.coords)
            x /= s;
        return result;
    }
}

namespace fmm
{
    using Vector2 = Vector_<2>;
    using Body2 = Body_<2>;
}

#endif