#ifndef BODY_HPP_
#define BODY_HPP_

#include "vector.hpp"
#include <iostream>

typedef  double massT;

template <typename T, int N>
class Body
{
private:
    massT mass;
    Vector<T, N> pos;
    Vector<T, N> vel;
    Vector<T, N> acc;

public:
    __host__ __device__ Body() {}

    __host__ __device__ Body(const massT& mass_init, const Vector<T, N>& pos_init, const Vector<T, N>& vel_init)
        : mass(mass_init), pos(pos_init), vel(vel_init) {}

    __host__ __device__ inline T getMass() const { return mass; }
    __host__ __device__ inline const Vector<T, N>& getPosition() const { return pos; }
    __host__ __device__ inline const Vector<T, N>& getVelocity() const { return vel; }
    __host__ __device__ inline const Vector<T, N>& getAcceleration() const { return acc; }

    __host__ __device__ inline void updatePosition(const Vector<T, N>& deltaPos) { pos += deltaPos; }
    __host__ __device__ inline void setPosition(const Vector<T, N>& newPos) { pos = newPos; }

    __host__ __device__ inline void updateVelocity(const Vector<T, N>& deltaVel) { vel += deltaVel; }
    __host__ __device__ inline void setVelocity(const Vector<T, N>& newVel) { vel = newVel; }

    __host__ __device__ inline void updateAcceleration(const Vector<T, N>& deltaAcc) { acc += deltaAcc; }
    __host__ __device__ inline void setAcceleration(const Vector<T, N>& newAcc) { acc = newAcc; }

    __host__ __device__ inline friend std::ostream& operator<<(std::ostream& os, const Body& body)
    {
        os << body.getPosition();
        return os;
    }
};
typedef Body<double, 2> Body2d;
typedef Body<double, 3> Body3d;
#endif