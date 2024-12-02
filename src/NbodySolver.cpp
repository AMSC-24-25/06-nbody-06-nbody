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
        // TODO: THIS PART CAN BE OPTIMIZED (SEE PACHECO PAGE 364)
        for (int q = 0; q < bodies.size(); q++)
        {
            for (int p = 0; p < bodies.size(); p++)
            {
                if (p != q)
                {
                    forces[q] += computeForce(bodies[q], bodies[p]);
                }
            }
        }
    }

    // Update bodies properties (position and velocity are updated using the old value)
    // Update positions
    {
        for (int q = 0; q < bodies.size(); q++)
        {
            bodies[q].updatePosition(deltaT * bodies[q].getVelocity());
        }
    }

    // Update velocities
    {
        for (int q = 0; q < bodies.size(); q++)
        {
            bodies[q].updateVelocity(deltaT * bodies[q].getAcceleration());
        }
    }

    // Update accelerations
    {
        for (int q = 0; q < bodies.size(); q++)
        {
            bodies[q].setAcceleration(1.0 / bodies[q].getMass() * forces[q]);
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
        std::cout << bodies[i].getPosition() << std::endl;
    }
}