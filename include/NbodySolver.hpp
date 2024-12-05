#ifndef NBODYSOLVER_HPP_
#define NBODYSOLVER_HPP_

#include <Body.hpp>
#include <IO.hpp>
#include <iostream>
#include <vector>

typedef double Real;

class NbodySolver
{
private:
    static const unsigned int dim = 2;
    static constexpr Real G = 1;
    unsigned int numBodies;
    std::vector<Body<Real, dim>> bodies;

    // Generic force term
    Vector<Real, dim> computeForce(const Body<Real, dim> &b1, const Body<Real, dim> &b2) const
    {
        Vector<Real, dim> r = b1.getPosition() - b2.getPosition();
        Real dist = r.norm();
        Real dist_cube = dist * dist * dist;
        r = -(G * b1.getMass() * b2.getMass() / dist_cube) * r;
        return r;
    }

public:
    // Used in pair with the addBody function, mostly for testing

    // Manually add a body, this is used mostly for testing
    void addBody(const massT &mass_, const Vector<Real, dim> &pos, const Vector<Real, dim> &vel)
    {
        Body<Real, dim> newBody(mass_, pos, vel);
        addBody(newBody);
    }

    void addBody(const Body<Real, dim> &body)
    {
        bodies.push_back(body);
        numBodies++;
    }

    // Load bodies from the file specified in the constructor
    void loadBodies(const std::string &bodies_file_name);

    // Executes one timestep of the simulation
    // DeltaT is specified here to allow varying-step simulations
    void step(const Real &deltaT);

    // Output bodies data to the specified file
    void outputData(std::ofstream &output_file) const;

    // Output timestep data to the specified file
    void outputTimestep(std::ofstream &output_file) const;
};

#endif