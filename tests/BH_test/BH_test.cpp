#include "Vector.hpp"
#include "Body.hpp"
#include "Quad.hpp"
#include "BHTree.hpp"
#include "NBodyBHSolver.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>

// Helper function to write positions to file for each timestep
template <typename T>
void writePositionsToFile(const std::vector<Body<T, 2>> &bodies, int timestep, std::ofstream &outFile)
{
    // Write timestep marker
    outFile << "# Timestep " << timestep << "\n";

    // Write positions and energy of each body
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        const auto &pos = bodies[i].getPosition();
        const auto energy = bodies[i].getEnergy();
        outFile << i << "\t" << pos[0] << "\t" << pos[1] << "\t" << energy << "\n";
    }
    outFile << "\n"; // Empty line between timesteps for easier parsing
}

int main()
{
    using T = double;

    // Define universe bounds
    T universeSize = 8.0;
    Vector<T, 2> origin({-universeSize / 2, -universeSize / 2});
    Quad<T> universe(origin, universeSize);

    // Create solver with appropriate timestep
    T timeStep = 1e-2;
    NBodyBHSolver<T> solver(universe, timeStep);

    // Planet data: mass (kg), distance from sun (m), orbital velocity (m/s)
    struct PlanetData
    {
        std::string name;
        T mass;
        T x;
        T y;
        T vx;
        T vy;
    };

    // Use data from https://numericaltank.sjtu.edu.cn/three-body/three-body-movies.htm
    // std::vector<PlanetData> planets = {
    //     {"A", 1.0, -1.0, 0.0, 0.3471168881, 0.5327249454},
    //     {"B", 1.0, 1.0, 0.0, 0.3471168881, 0.5327249454},
    //     {"C", 1.0, 0.0, 0.0, -2 * 0.3471168881, -2 * 0.5327249454}};

    // This is I.A_3^{i.c} test case
    std::vector<PlanetData> planets = {
        {"A", 1.0, -1.0, 0.0, 0.6150407229, 0.5226158545},
        {"B", 1.0, 1.0, 0.0, 0.6150407229, 0.5226158545},
        {"C", 1.0, 0.0, 0.0, -2 * 0.6150407229, -2 * 0.5226158545}};

    // This is I.A_2^{i.c} test case (with collision)
    // std::vector<PlanetData> planets = {
    //     {"A", 1.0, -1.0, 0.0, 0.3068934205, 0.1255065670},
    //     {"B", 1.0, 1.0, 0.0, 0.3068934205, 0.1255065670},
    //     {"C", 1.0, 0.0, 0.0, -2 * 0.3068934205, -2 * 0.1255065670}};

    // Add bodies to the simulation
    for (const auto &planet : planets)
    {
        Vector<T, 2> position({planet.x, planet.y});
        Vector<T, 2> velocity({planet.vx, planet.vy});

        solver.addBody(Body<T, 2>(planet.mass, position, velocity));
    }

    // Open output file
    std::ofstream outFile("../particle_positions.txt");
    if (!outFile)
    {
        std::cerr << "Failed to open output file" << std::endl;
        return 1;
    }

    const int numParticles = planets.size();

    // Write header
    outFile << "# Format: ParticleID\tX\tY\n";
    outFile << "# Number of particles: " << numParticles << "\n\n";

    // Run simulation for 100s (also see 1000s with timestep 1e-4)
    const int numSteps = 100000;
    std::cout << "Starting simulation with " << numParticles << " particles for " << numSteps << " steps\n";

    // Write initial positions
    solver.calculateEnergy();
    writePositionsToFile(solver.getBodies(), 0, outFile);

    // Simulation loop
    for (int step = 1; step <= numSteps; ++step)
    {
        solver.simulateOneStep();
        solver.calculateEnergy();

        // Write positions every 0.1s
        if (step % 10 == 0)
        {
            writePositionsToFile(solver.getBodies(), step, outFile);
            std::cout << "Completed step " << step << "/" << numSteps << "\n";
        }
    }

    outFile.close();
    std::cout << "Simulation complete. Results written to particle_positions.txt\n";
    return 0;
}