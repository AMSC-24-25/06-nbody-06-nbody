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
            // Reset the vector for the next iteration
            forces[i][q] = Vector<Real, dim>();
        }

        // Leapfrog integrator
        bodies[q].setAcceleration(totForce / bodies[q].getMass());
        bodies[q].updateVelocity(bodies[q].getAcceleration() * deltaT);
        bodies[q].updatePosition(bodies[q].getVelocity() * deltaT);
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

const Real NbodySolver::computeEnergy() const
{
    Real K = 0.0;
    // Kinetic component
    for (const auto &body : bodies)
    {
        K += 0.5 * body.getMass() * pow(body.getVelocity().norm(), 2.0);
    }

    Real U = 0.0;
    // Potential component
    for (int q = 0; q < bodies.size(); q++)
    {
        for (int p = q + 1; p < bodies.size(); p++)
        {
            U += -G * bodies[p].getMass() * bodies[q].getMass() / ((bodies[q].getPosition() - bodies[p].getPosition()).norm());
        }
    }

    return K + U;
}

void NbodySolver::initSharedVar()
{
    forces.resize(omp_get_num_threads(), std::vector<Vector<Real, dim>>(numBodies));
}