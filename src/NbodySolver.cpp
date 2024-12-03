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
            for (int p = 0; p < bodies.size(); p++)
            {
                if (p != q)
                {
                    forces[q] += computeForce(bodies[q], bodies[p]);
                    // forces[p] -= computeForce(bodies[q], bodies[p]);
                }
            }
        }
    }

    // Update bodies properties (position and velocity are updated using the old value)
    {
        for (int q = 0; q < bodies.size(); q++)
        {
            //Backward Euler (suffers from energy drift)
            bodies[q].updatePosition(deltaT * bodies[q].getVelocity());
            bodies[q].updateVelocity(deltaT * bodies[q].getAcceleration());
            bodies[q].setAcceleration(1.0 / bodies[q].getMass() * forces[q]);

            // std::cout << bodies[q].getAcceleration() << std::endl;

            //Symplectic Euler (still some energy drift)
            // bodies[q].updateVelocity(deltaT * bodies[q].getAcceleration());
            // bodies[q].updatePosition(deltaT * bodies[q].getVelocity());
            // bodies[q].setAcceleration(1.0 / bodies[q].getMass() * forces[q]);

            //Leapfrog algorithm
            // bodies[q].updatePosition(deltaT*bodies[q].getVelocity()+0.5*deltaT*deltaT*bodies[q].getAcceleration());
            // auto acc_new = 1.0 / bodies[q].getMass() * forces[q];
            // bodies[q].updateVelocity(deltaT*0.5*(bodies[q].getAcceleration() + acc_new));
            // bodies[q].setAcceleration(acc_new);
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
        std::cout << bodies[i].getPosition() << std::endl << bodies[i].getAcceleration() << std::endl;
    }
}