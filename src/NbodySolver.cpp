#include <NbodySolver.hpp>
#include <iostream>

void NbodySolver::setup()
{
    // TODO: HERE IT SHOULD LOAD THE BODIES FROM FILE AND PUT THEM IN THE VECTOR
}

void NbodySolver::step()
{
    std::vector<Vector<double, dim>> forces(numBodies);
    // Compute forces
    {
        // TODO: THIS IS THE PART THAT CAN BE PARALLELIZED
        for (int q = 0; q < bodies.size(); q++)
        {
            for (int p = q + 1; p < bodies.size(); p++)
            {
                forces[q] += computeForce(bodies[q], bodies[p]);
                forces[p] -= computeForce(bodies[q], bodies[p]);
            }
        }
    }

    // Update bodies properties (position and velocity are updated using the old value)
    {
        for (int q = 0; q < bodies.size(); q++)
        {
            // Leapfrog KDK integrator
            bodies[q].updateVelocity(0.5 * bodies[q].getAcceleration() * deltaT);
            bodies[q].updatePosition(bodies[q].getVelocity() * deltaT);
            bodies[q].setAcceleration(forces[q] / bodies[q].getMass());
            bodies[q].updateVelocity(0.5 * bodies[q].getAcceleration() * deltaT);
        }
    }
}

void NbodySolver::output(const std::string &output_file_name)
{
    // TODO: HERE IT SHOULD TAKE THE VECTOR AND SAVE IT TO A FILE, WHICH WILL BE FED IN THE VISUALIZER
}

void NbodySolver::output()
{
    for (int i = 0; i < bodies.size(); i++)
    {
        std::cout << bodies[i].getPosition() << std::endl
                  << bodies[i].getAcceleration() << std::endl;
    }
}