#ifndef COLLISION_HANDLER_HPP
#define COLLISION_HANDLER_HPP

#include "Body.hpp"
#include "Vector.hpp"
#include <optional>

template <typename T>
class CollisionHandler
{
private:
    static constexpr T COLLISION_DISTANCE = 1e-6;
    static constexpr T EPSILON = 1e-8;
    static constexpr T RESTITUTION = 1.0; // Slightly inelastic collisions (ideal elastic here)

    static std::optional<T> getCollisionTime(const Body<T, 2> &body1, const Body<T, 2> &body2, T timeStep)
    {
        Vector<T, 2> r = body1.getPosition() - body2.getPosition();
        Vector<T, 2> v = body1.getVelocity() - body2.getVelocity();

        // Quick rejection if bodies are moving apart
        if (r.dot(v) >= 0)
            return std::nullopt;

        // Solve quadratic equation for collision time
        T a = v.dot(v);
        T b = 2 * r.dot(v);
        T c = r.dot(r) - COLLISION_DISTANCE * COLLISION_DISTANCE;

        T discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            return std::nullopt;

        T t = (-b - std::sqrt(discriminant)) / (2 * a);
        return (t > 0 && t < timeStep) ? std::optional<T>(t) : std::nullopt;
    }

    static Vector<T, 2> calculateCollisionVelocity(const Body<T, 2> &body1, const Body<T, 2> &body2)
    {
        T m1 = body1.getMass();
        T m2 = body2.getMass();
        Vector<T, 2> v1 = body1.getVelocity();
        Vector<T, 2> v2 = body2.getVelocity();
        Vector<T, 2> r = body1.getPosition() - body2.getPosition();

        // Normalize collision normal
        r = r / r.norm();

        // Project velocities onto collision normal
        T v1n = v1.dot(r);
        T v2n = v2.dot(r);

        // Calculate new normal velocity with restitution
        T v1n_new = (m1 * v1n + m2 * v2n + m2 * RESTITUTION * (v2n - v1n)) / (m1 + m2);

        // Keep tangential component
        Vector<T, 2> v1t = v1 - r * v1n;

        return v1t + r * v1n_new;
    }

public:
    static bool handleCollision(Body<T, 2> &body1, Body<T, 2> &body2, T timeStep)
    {
        auto collisionTime = getCollisionTime(body1, body2, timeStep);
        if (!collisionTime)
            return false;

        T ct = *collisionTime;

        // Move to collision point
        body1.updatePosition(body1.getVelocity() * ct);
        body2.updatePosition(body2.getVelocity() * ct);

        // Calculate and update velocities
        Vector<T, 2> v1_new = calculateCollisionVelocity(body1, body2);
        Vector<T, 2> v2_new = calculateCollisionVelocity(body2, body1);
        body1.setVelocity(v1_new);
        body2.setVelocity(v2_new);

        // Move for remaining time
        T remaining = timeStep - ct;
        if (remaining > EPSILON)
        {
            body1.updatePosition(v1_new * remaining);
            body2.updatePosition(v2_new * remaining);
        }

        return true;
    }
};

#endif // COLLISION_HANDLER_HPP