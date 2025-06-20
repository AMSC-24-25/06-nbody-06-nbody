#include "Vector.hpp"
#include "Body.hpp"
#include "Quad.hpp"
#include "BHTree.hpp"
#include "NBodyBHSolver.hpp"
#include "RandomBodyGenerator.hpp"
#include "Entropy.hpp" 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

// Write positions (no energy) to file
template <typename T>
void writePositionsToFile(const std::vector<Body<T, 2>> &bodies, int timestep, std::ofstream &outFile)
{
    outFile << "# Timestep " << timestep << "\n";
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        const auto &pos = bodies[i].getPosition();
        outFile << std::setw(6) << i
                << std::setw(15) << std::fixed << std::setprecision(8) << pos[0]
                << std::setw(15) << std::fixed << std::setprecision(8) << pos[1]
                << "\n";
    }
    outFile << "\n";
}

// Write total system energy (sum of body.energy) to file
template <typename T>
void writeEnergyToFile(const std::vector<Body<T, 2>> &bodies, int timestep, std::ofstream &energyFile)
{
    T totalEnergy = 0;
    for (const auto &body : bodies)
        totalEnergy += body.getEnergy();
    energyFile << "# Timestep " << timestep << "\n";
    energyFile << std::setprecision(12) << totalEnergy << "\n\n";
}

template <typename T>
void displaySystemStats(const std::vector<Body<T, 2>> &bodies)
{
    T totalMass = 0;
    T avgVelocity = 0;
    T totalKineticEnergy = 0;
    Vector<T, 2> centerOfMass({0, 0});

    for (const auto &body : bodies)
    {
        totalMass += body.getMass();
        avgVelocity += body.getVelocity().norm();
        totalKineticEnergy += 0.5 * body.getMass() * body.getVelocity().norm() * body.getVelocity().norm();
        centerOfMass = centerOfMass + body.getPosition() * body.getMass();
    }

    centerOfMass = centerOfMass / totalMass;
    avgVelocity /= bodies.size();

    std::cout << "System Statistics:\n";
    std::cout << "  Total mass: " << std::fixed << std::setprecision(3) << totalMass << "\n";
    std::cout << "  Average velocity: " << std::fixed << std::setprecision(3) << avgVelocity << "\n";
    std::cout << "  Total kinetic energy: " << std::scientific << std::setprecision(3) << totalKineticEnergy << "\n";
    std::cout << "  Center of mass: (" << std::fixed << std::setprecision(3)
              << centerOfMass[0] << ", " << centerOfMass[1] << ")\n\n";
}

int main()
{
    using T = double;

    // Simulation parameters
    const int numBodies = 100;
    const int numSteps = 10000;
    const int outputInterval = 1000;
    const T universeSize = 10.0;
    const T timeStep = 1e-4;

    Vector<T, 2> origin({-universeSize / 2, -universeSize / 2});
    Quad<T> universe(origin, universeSize);
    NBodyBHSolver<T> solver(universe, timeStep);

    std::cout << "Generating " << numBodies << " bodies...\n";
    auto system = nbody::generateStableRing(numBodies, universeSize, 100.0, 12345, 1.0);

    for (const auto &body : system.bodies)
        solver.addBody(body);

    displaySystemStats(solver.getBodies());

    std::ofstream posFile("../particle_positions.txt");
    std::ofstream energyFile("../energy_evolution.txt");
    std::ofstream entropyFile("../entropy_log.txt");
    if (!posFile || !energyFile || !entropyFile)
    {
        std::cerr << "Failed to open output files" << std::endl;
        return 1;
    }

    // Write headers
    posFile << "# N-Body Simulation Particle Positions\n";
    posFile << "# Format: ParticleID\tX\tY\n";
    posFile << "# Number of particles: " << numBodies << "\n";
    posFile << "# Time step: " << timeStep << "\n";
    posFile << "# Universe size: " << universeSize << "\n";
    posFile << "# Total steps: " << numSteps << "\n\n";

    energyFile << "# N-Body Simulation Energy Evolution\n";
    energyFile << "# Format: TotalEnergy\n";
    energyFile << "# Number of particles: " << numBodies << "\n";
    energyFile << "# Time step: " << timeStep << "\n";
    energyFile << "# Universe size: " << universeSize << "\n";
    energyFile << "# Total steps: " << numSteps << "\n\n";

    entropyFile << "# Step\tEntropy\n";

    std::cout << "Starting N-Body simulation with Barnes-Hut algorithm\n";
    std::cout << "Configuration:\n";
    std::cout << "  Bodies: " << numBodies << "\n";
    std::cout << "  Steps: " << numSteps << "\n";
    std::cout << "  Time step: " << timeStep << "\n";
    std::cout << "  Universe size: " << universeSize << "\n";
    std::cout << "  Output interval: " << outputInterval << "\n\n";

    // Initial state
    solver.calculateEnergy();
    writePositionsToFile(solver.getBodies(), 0, posFile);
    writeEnergyToFile(solver.getBodies(), 0, energyFile);

    // --- Entropy grid setup ---
    const int gridSize = 1000;
    std::vector<std::vector<int>> entropyGrid(gridSize, std::vector<int>(gridSize, 0));
    std::size_t sampleCount = 0;

    T minX = -universeSize / 2;
    T maxX = universeSize / 2;
    T minY = -universeSize / 2;
    T maxY = universeSize / 2;
    T invDx = gridSize / (maxX - minX);
    T invDy = gridSize / (maxY - minY);

    for (int step = 1; step <= numSteps; ++step)
    {
        solver.simulateOneStep();

        // Update entropy grid for all particles
        updateEntropyGrid(solver.getBodies(), entropyGrid, minX, maxX, minY, maxY, invDx, invDy);
        sampleCount += solver.getBodies().size();

        if (step % outputInterval == 0)
        {
            solver.calculateEnergy();
            writePositionsToFile(solver.getBodies(), step, posFile);
            writeEnergyToFile(solver.getBodies(), step, energyFile);

            double entropy = computeNormalizedEntropy(entropyGrid, sampleCount);
            entropyFile << step << "\t" << entropy << "\n";

            double progress = (double)step / numSteps * 100.0;
            std::cout << "Step " << std::setw(6) << step
                      << "/" << numSteps
                      << " (" << std::fixed << std::setprecision(1) << progress << "%)\n";
        }
    }

    posFile.close();
    energyFile.close();
    entropyFile.close();

    std::cout << "\nSimulation completed successfully!\n";
    std::cout << "Results written to particle_positions.txt, energy_evolution.txt, and entropy_log.txt\n";

    std::cout << "\nFinal ";
    displaySystemStats(solver.getBodies());

    return 0;
}
