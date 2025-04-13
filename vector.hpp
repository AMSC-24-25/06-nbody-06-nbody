#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#include <cmath>
#include <stdexcept>
#include <iostream>

template <typename T, int N>
class Vector
{
private:
    T comps[N];

public:
    static const Vector ZERO;

    __host__ __device__ Vector() {
        for (int i = 0; i < N; ++i)
            comps[i] = T(0);
    }
    __host__ __device__ Vector(T x, T y) {
        comps[0] = x;
        comps[1] = y;
    }


    __host__ __device__ Vector(const T& val) {
        for (int i = 0; i < N; ++i)
            comps[i] = val;
    }

    __host__ __device__ Vector(const T* init) {
        for (int i = 0; i < N; ++i)
            comps[i] = init[i];
    }

    __host__ __device__ Vector(const Vector& other) {
        for (int i = 0; i < N; ++i)
            comps[i] = other[i];
    }

    // Element access
    __host__ __device__ T& operator[](int i) {
        return comps[i];
    }

    __host__ __device__ const T& operator[](int i) const {
        return comps[i];
    }

    // Arithmetic operators
    __host__ __device__ Vector& operator+=(const Vector& rhs) {
        for (int i = 0; i < N; ++i)
            comps[i] += rhs[i];
        return *this;
    }

    __host__ __device__ Vector& operator-=(const Vector& rhs) {
        for (int i = 0; i < N; ++i)
            comps[i] -= rhs[i];
        return *this;
    }

    __host__ __device__ friend Vector operator+(const Vector& a, const Vector& b) {
        Vector result = a;
        result += b;
        return result;
    }

    __host__ __device__ friend Vector operator-(const Vector& a, const Vector& b) {
        Vector result = a;
        result -= b;
        return result;
    }

    __host__ __device__ friend Vector operator*(const T& scalar, const Vector& v) {
        Vector result;
        for (int i = 0; i < N; ++i)
            result[i] = scalar * v[i];
        return result;
    }

    __host__ __device__ friend Vector operator*(const Vector& v, const T& scalar) {
        return scalar * v;
    }

    __host__ __device__ friend Vector operator/(const Vector& v, const T& scalar) {
        Vector result;
        for (int i = 0; i < N; ++i)
            result[i] = v[i] / scalar;
        return result;
    }

    __host__ __device__ T norm() const {
        T sum = T(0);
        for (int i = 0; i < N; ++i)
            sum += comps[i] * comps[i];
        return sqrt(sum);
    }

    // Optional: CPU-only output
    friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
        for (int i = 0; i < N; ++i) {
            os << v[i];
            if (i < N - 1)
                os << " ";
        }
        return os;
    }
};

// Aliases
typedef Vector<double, 2> Vector2d;
typedef Vector<double, 3> Vector3d;
#endif