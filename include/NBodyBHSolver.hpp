#ifndef NBODYBHSOLVER_HPP_
#define NBODYBHSOLVER_HPP_

#include "Vector.hpp"
#include "Body.hpp"
#include "Quad.hpp"
#include "BHTree.hpp"
#include <vector>
#include <cmath>

template <typename T>
class NBodyBHSolver
{
private:
    std::vector<Body<T, 2>> bodies;
    Quad<T> universe;
    T timeStep;

public:
    NBodyBHSolver(const Quad<T> &universe, T timeStep = 0.1)
        : universe(universe), timeStep(timeStep) {}

    void addBody(const Body<T, 2> &body)
    {
        bodies.push_back(body);
    }

    void simulateOneStep()
    {
        // First force calculation
        BHTree<T> tree1(universe);
        for (Body<T, 2> &body : bodies)
        {
            if (universe.contains(body.getPosition()))
            {
                tree1.insert(body);
            }
        }

        // Reset and calculate initial accelerations
        for (Body<T, 2> &body : bodies)
        {
            body.setAcceleration(Vector<T, 2>());
            tree1.updateForce(body);
        }

        // Step 1: First Kick
        for (Body<T, 2> &body : bodies)
        {
            Vector<T, 2> deltaVel = body.getAcceleration() * timeStep * 0.5;
            body.updateVelocity(deltaVel);
        }

        // Step 2: Drift
        for (Body<T, 2> &body : bodies)
        {
            Vector<T, 2> deltaPos = body.getVelocity() * timeStep;
            body.updatePosition(deltaPos);
            // Todo: Handle boundary conditions?
            // if (!universe.contains(body.getPosition()))
            // {
            //   Periodic boundaries or elastic collision?
        }

        // Rebuild tree and recalculate forces at new positions
        BHTree<T> tree2(universe);
        for (Body<T, 2> &body : bodies)
        {
            if (universe.contains(body.getPosition()))
            {
                tree2.insert(body);
            }
        }

        // Calculate new accelerations
        for (Body<T, 2> &body : bodies)
        {
            body.setAcceleration(Vector<T, 2>());
            tree2.updateForce(body);
        }

        // Step 3: Second Kick
        for (Body<T, 2> &body : bodies)
        {
            Vector<T, 2> deltaVel = body.getAcceleration() * timeStep * 0.5;
            body.updateVelocity(deltaVel);
        }
    }

    const std::vector<Body<T, 2>> &getBodies() const
    {
        return bodies;
    }

    void output()
    {
        for (size_t i = 0; i < bodies.size(); i++)
        {
            std::cout << bodies[i].getPosition() << std::endl
                      << bodies[i].getAcceleration() << std::endl;
        }
    }
};

#endif // NBODYBHSOLVER_HPP_