#ifndef NBODYSOLVER_HPP_
#define NBODYSOLVER_HPP_

#include <Body.hpp>
#include <IO.hpp>
#include <iostream>
#include <vector>
#include <omp.h>

typedef double Real;

class NbodySolver
{
private:
    static const unsigned int dim = 2;
    static constexpr Real G = 1;
    unsigned int numBodies;
    std::vector<Body<Real, dim>> bodies;
    mutable std::vector<std::vector<Vector<Real, dim>>> forces;

    // Generic force term
    inline Vector<Real, dim> computeForce(const Body<Real, dim> &b1, const Body<Real, dim> &b2) const
    {
        Vector<Real, dim> r = b1.getPosition() - b2.getPosition();
        Real dist = r.norm();
        Real dist_cube = dist * dist * dist;
        r = -(G * b1.getMass() * b2.getMass() / dist_cube) * r;
        return r;
    }

public:
    // Add body given its parameters
    void addBody(const massT &mass_, const Vector<Real, dim> &pos, const Vector<Real, dim> &vel);

    // Add an existing body
    void addBody(const Body<Real, dim> &body);

    // Load bodies from the file specified in the constructor
    void loadBodies(const std::string &bodies_file_name);

    // Executes one timestep of the simulation
    // DeltaT is specified here to allow varying-step simulations
    void step(const Real &deltaT);

    // Output bodies data to the specified file
    void outputData(std::ofstream &output_file) const;

    // Output timestep data to the specified file
    void outputTimestep(std::ofstream &output_file) const;

    // Compute initial energy, used for error evaluation
    const Real computeEnergy() const;

    //Initialize the forces vector used by the OpenMP processes
    void initSharedVar() const;
};

#endif