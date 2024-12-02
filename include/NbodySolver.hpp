#ifndef NBODYSOLVER_HPP_
#define NBODYSOLVER_HPP_

#include <Body.hpp>
#include <vector>

class NbodySolver
{
private:
    static const unsigned int dim = 1;
    const unsigned int numBodies;
    std::vector<Body<double, dim>> bodies;
    const std::string bodies_file_name;
    const double deltaT;

    // Generic force term
    Vector<double, dim> computeForce(const Body<double, dim> &b1, const Body<double, dim> &b2) const
    {
        Vector<double, dim> r = b1.getPosition() - b2.getPosition();
        double dist = r.norm();
        double dist_cube = dist * dist * dist;
        r = (6.67e-11 * b1.getMass() * b2.getMass() / dist_cube) * r;
        return r;
    }

public:
    // Used in pair with the addBody function, mostly for testing
    NbodySolver(const unsigned int &numBodies_, const double &deltaT_) : numBodies(numBodies_),
                                                                         deltaT(deltaT_) {}
    // NbodySolver(const std::string &bodies_file_name_, const double &deltaT_) : bodies_file_name(bodies_file_name_),
    //                                                                            deltaT(deltaT_) {}

    // Manually add a body, this is used mostly for testing
    void addBody(const massT &mass_, const Vector<double, dim> &pos, const Vector<double, dim> &vel)
    {
        Body<double, dim> newBody(mass_, pos, vel);
        bodies.push_back(newBody);
    }

    void addBody(const Body<double, dim> &body)
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