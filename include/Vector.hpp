#ifndef VECTOR_H
#define VECTOR_H

#include <array>
#include <iostream>

template <typename T, size_t N>
class Vector
{
private:
    std::array<T, N> comps;

public:
    // Default vectors
    static const Vector ZERO;
    // Constructor
    Vector() { comps.fill(0); }
    Vector(const std::initializer_list<T> &init)
    {
        std::copy(init.begin(), init.end(), comps.begin());
    }

    // Arithmetic operators
    inline friend Vector operator+(const Vector &lhs, const Vector &rhs)
    {
        Vector res;
        for (int i = 0; i < N; i++)
        {
            res[i] = lhs[i] + rhs[i];
        }
        return res;
    }
    Vector &operator+=(const Vector &rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    inline friend Vector<T, N> operator-(const Vector &lhs, const Vector &rhs)
    {
        Vector res;
        for (int i = 0; i < N; i++)
        {
            res[i] = lhs[i] - rhs[i];
        }
        return res;
    }
    Vector &operator-=(const Vector &rhs)
    {
        *this = *this - rhs;
        return *this;
    }

    inline friend Vector operator*(const T &scalar, const Vector &rhs)
    {
        Vector res;
        for (int i = 0; i < N; i++)
        {
            res[i] = rhs[i] * scalar;
        }

        return res;
    }

    // WARNING: THIS IS DANGEROUS WHEN WORKING WITH FLOATS AND DOUBLES
    inline friend bool operator==(const Vector &lhs, const Vector &rhs)
    {
        for (int i = 0; i < N; i++)
            if (lhs[i] != rhs[i])
                return false;
        return true;
    }
    // Vector utilities
    inline const T norm() const
    {
        T norm = 0;
        for (int i = 0; i < N; i++)
        {
            norm += comps[i] * comps[i];
        }

        return sqrt(norm);
    }

    // Access operators
    inline T &operator[](size_t coord)
    {
        return comps[coord];
    }
    inline const T &operator[](size_t coord) const
    {
        return comps[coord];
    }

    // Stream operators
    inline friend std::ostream &operator<<(std::ostream &os, const Vector &vec)
    {
        os << "(";
        for (std::size_t i = 0; i < N; ++i)
        {
            os << vec.comps[i];
            if (i < N - 1)
                os << ", ";
        }
        os << ")";
        return os;
    }
};

// Definition of some useful types
typedef Vector<double, 2> Vector2d;
typedef Vector<double, 3> Vector3d;

#endif