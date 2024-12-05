#include <NbodySolver.hpp>


void NbodySolver::loadBodies(const std::string &bodies_file_name)
{
    bodies = IO<Real, dim>::readBodiesFromFile(bodies_file_name);
    numBodies = bodies.size();
}

void NbodySolver::step(const Real &deltaT)
{
    std::vector<Vector<Real, dim>> forces(numBodies);
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

void NbodySolver::outputData(std::ofstream &output_file) const
{
    IO<Real,dim>::writeBodiesInfo(output_file,bodies);
}

void NbodySolver::outputTimestep(std::ofstream &output_file) const
{
    IO<Real,dim>::writeSimulationData(output_file,bodies);
}