#include <NbodySolver.hpp>
#include <omp.h>

void NbodySolver::loadBodies(const std::string &bodies_file_name)
{
    bodies = IO<Real, dim>::readBodiesFromFile(bodies_file_name);
    numBodies = bodies.size();
}

void NbodySolver::step(const Real &deltaT)
{
    // Compute (local) forces
    #pragma omp for
    for (int q = 0; q < bodies.size(); q++)
    {
        for (int p = q + 1; p < bodies.size(); p++)
        {
            Vector<Real, dim> qpForce = computeForce(bodies[q], bodies[p]);
            forces[omp_get_thread_num()][q] += qpForce;
            forces[omp_get_thread_num()][p] -= qpForce;
        }
    }

    // Update bodies properties (position and velocity are updated using the old value)
    #pragma omp for
    for (int q = 0; q < bodies.size(); q++)
    {
        // Compute total force from local forces
        Vector<Real, dim> totForce;
        for (int i = 0; i < omp_get_num_threads(); i++)
        {
            totForce += forces[i][q];
            //Reset the vector for the next iteration
            forces[i][q] = Vector<Real,dim>();
        }
        // Leapfrog KDK integrator
        bodies[q].updateVelocity(0.5 * bodies[q].getAcceleration() * deltaT);
        bodies[q].updatePosition(bodies[q].getVelocity() * deltaT);
        bodies[q].setAcceleration(totForce / bodies[q].getMass());
        bodies[q].updateVelocity(0.5 * bodies[q].getAcceleration() * deltaT);
    }
}

void NbodySolver::outputData(std::ofstream &output_file) const
{
    IO<Real, dim>::writeBodiesInfo(output_file, bodies);
}

void NbodySolver::outputTimestep(std::ofstream &output_file) const
{
    IO<Real, dim>::writeSimulationData(output_file, bodies);
}