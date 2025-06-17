#ifndef NBODYBHSOLVER_OPENMP_HPP_
#define NBODYBHSOLVER_OPENMP_HPP_

#include "Vector.hpp"
#include "Body.hpp"
#include "Quad.hpp"
#include "BHTree.hpp"
#include "CollisionHandler.hpp"
#include <vector>
#include <cmath>
#include <omp.h>

template <typename T>
class NBodyBHSolverOpenMP
{
private:
    std::vector<Body<T, 2>> bodies;
    Quad<T> universe;
    T timeStep;

public:
    NBodyBHSolverOpenMP(const Quad<T> &universe, T timeStep = 0.1)
        : universe(universe), timeStep(timeStep) {}

    const std::vector<Body<T, 2>> &getBodies() const
    {
        return bodies;
    }

    void addBody(const Body<T, 2> &body)
    {
        bodies.push_back(body);
    }

    void calculateEnergy()
    {
        // Parallel over bodies[i], each thread calculates its own energy
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < bodies.size(); ++i) {
            T newKineticEnergy = 0.5 * bodies[i].getMass() * bodies[i].getVelocity().norm() * bodies[i].getVelocity().norm();
            T newPotentialEnergy = 0.0;
            for (size_t j = 0; j < bodies.size(); ++j) {
                if (i != j) {
                    Vector<T, 2> r = bodies[i].getPosition() - bodies[j].getPosition();
                    newPotentialEnergy -= bodies[i].getMass() * bodies[j].getMass() / r.norm();
                }
            }
            bodies[i].setEnergy(newKineticEnergy + newPotentialEnergy / 2.0);
        }
    }

    void simulateOneStep()
    {
        // First force calculation
        // Tree construction must be sequential
        BHTree<T> tree1(universe);
        for (Body<T, 2> &body : bodies)
        {
            if (universe.contains(body.getPosition()))
            {
                tree1.insert(body);
            }
        }

        // Reset and calculate initial accelerations
        // Parallel force computaion (This is safe since each body is independent)
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < bodies.size(); ++i) {
            bodies[i].setAcceleration(Vector<T, 2>());
            tree1.updateForce(bodies[i]);
        }

        // Step 1: First Kick
        // safe per body
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < bodies.size(); ++i) {
            Vector<T, 2> deltaVel = bodies[i].getAcceleration() * timeStep * 0.5;
            bodies[i].updateVelocity(deltaVel);
        }

        // Step 2: Drift
        // safe per body
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < bodies.size(); ++i) {
            Vector<T, 2> deltaPos = bodies[i].getVelocity() * timeStep;
            bodies[i].updatePosition(deltaPos);
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
        // Parallel 
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < bodies.size(); ++i) {
            bodies[i].setAcceleration(Vector<T, 2>());
            tree2.updateForce(bodies[i]);
        }

        // Step 3: Second Kick
        // safe per body
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < bodies.size(); ++i) {
            Vector<T, 2> deltaVel = bodies[i].getAcceleration() * timeStep * 0.5;
            bodies[i].updateVelocity(deltaVel);
        }

        // Handle collisions
        // Sequential
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            for (size_t j = i + 1; j < bodies.size(); ++j)
            {
                CollisionHandler<T>::handleCollision(bodies[i], bodies[j], timeStep);
            }
        }
    }
};

#endif // NBODYBHSOLVER_OPENMP_HPP_