#ifndef NBODYSOLVER_HPP_
#define NBODYSOLVER_HPP_

#include <Body.hpp>
#include <vector>

typedef double Real;

class NbodySolver
{
private:
    static const unsigned int dim = 2;
    static constexpr Real G = 1;
    const unsigned int numBodies;
    std::vector<Body<Real, dim>> bodies;
    const std::string bodies_file_name;
    const Real deltaT;

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
    NbodySolver(const unsigned int &numBodies_, const Real &deltaT_) : numBodies(numBodies_),
                                                                         deltaT(deltaT_) {}
    // NbodySolver(const std::string &bodies_file_name_, const Real &deltaT_) : bodies_file_name(bodies_file_name_),
    //                                                                            deltaT(deltaT_) {}

    // Manually add a body, this is used mostly for testing
    void addBody(const massT &mass_, const Vector<Real, dim> &pos, const Vector<Real, dim> &vel)
    {
        Body<Real, dim> newBody(mass_, pos, vel);
        bodies.push_back(newBody);
    }

    void addBody(const Body<Real, dim> &body)
    {
        bodies.push_back(body);
    }

    // Load bodies from the file specified in the constructor
    void setup();

    // Executes one timestep of the simulation
    void step();

    // Output bodies information to the specified file
    void output(const std::string &output_file_name);

    // Output bodies information to cout, used mostly for testing
    void output();
};

#endif