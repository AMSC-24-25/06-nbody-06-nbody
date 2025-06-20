#include "Vector.hpp"
#include "Body.hpp"
#include "Quad.hpp"
#include "BHTree.hpp"
#include "NBodyBHSolverOpenMP.hpp"
typedef NBodyBHSolverOpenMP<double> SolverType;
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include <chrono>
#include <omp.h>       // For omp_get_max_threads()

// Helper function to write positions to file for each timestep
template <typename T>
void writePositionsToFile(const std::vector<Body<T, 2>> &bodies, int timestep, std::ofstream &outFile)
{
    outFile << "# Timestep " << timestep << "\n";
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        const auto &pos = bodies[i].getPosition();
        const auto energy = bodies[i].getEnergy();
        outFile << i << "\t" << pos[0] << "\t" << pos[1] << "\t" << energy << "\n";
    }
    outFile << "\n";
}

template <typename T>
void updateEntropyGrid(const std::vector<Body<T, 2>> &bodies, std::vector<std::vector<int>> &grid,
                       T minX, T maxX, T minY, T maxY, T invDx, T invDy)
{
    int size = grid.size();
    for (const auto &body : bodies)
    {
        auto pos = body.getPosition();
        int i = static_cast<int>((pos[0] - minX) * invDx);
        int j = static_cast<int>((pos[1] - minY) * invDy);
        if (i >= 0 && i < size && j >= 0 && j < size)
        {
            grid[i][j]++;
        }
    }
}

double computeNormalizedEntropy(const std::vector<std::vector<int>> &grid, std::size_t totalSamples, double k_B = 1.0)
{
    double entropy = 0.0;
    int numBins = grid.size() * grid[0].size(); // Total number of grid cells

    for (const auto &row : grid)
    {
        for (int count : row)
        {
            if (count > 0)
            {
                double p = static_cast<double>(count) / static_cast<double>(totalSamples);
                entropy -= p * std::log(p);
            }
        }
    }

    double maxEntropy = std::log(static_cast<double>(numBins));
    return (maxEntropy > 0) ? (entropy / maxEntropy) * k_B : 0.0;
}

int main()
{
    using T = double;

    T universeSize = 3.0;
    Vector<T, 2> origin({-universeSize / 2, -universeSize / 2});
    Quad<T> universe(origin, universeSize);

    T timeStep = 1e-5;
    // NBodyBHSolver<T> solver(universe, timeStep);
    SolverType solver(universe, timeStep);

    struct PlanetData
    {
        std::string name;
        T mass;
        T x;
        T y;
        T vx;
        T vy;
    };

    // std::vector<PlanetData> planets = {
    //     {"A", 1.0, -1.0, 0.0, 0.3068934205, 0.1255065670},
    //     {"B", 1.0, 1.0, 0.0, 0.3068934205, 0.1255065670},
    //     {"C", 1.0, 0.0, 0.0, -2 * 0.3068934205, -2 * 0.1255065670}};
    std::vector<PlanetData> planets;
    // std::ifstream in("generated_bodies.txt");
    std::ifstream in("GeneratedBodies/generated_bodies_500.txt");
    if (!in)
    {
        std::cerr << "Failed to open input file" << std::endl;
        return 1;
    }
    
    double mass, x, y, vx, vy;
    int id = 0;

    while (in >> x >> y >> vx >> vy >> mass) {
        PlanetData p;
        p.name = "P" + std::to_string(id++);
        p.mass = mass;
        p.x = x;
        p.y = y;
        p.vx = vx;
        p.vy = vy;
        planets.push_back(p);
    }

    // std::vector<PlanetData> planets = {
    //     {"A", 1.0, -1.0, 0.0, 0.3471168881, 0.5327249454},
    //     {"B", 1.0, 1.0, 0.0, 0.3471168881, 0.5327249454},
    //     {"C", 1.0, 0.0, 0.0, -2 * 0.3471168881, -2 * 0.5327249454}};

    for (const auto &planet : planets)
    {
        Vector<T, 2> position({planet.x, planet.y});
        Vector<T, 2> velocity({planet.vx, planet.vy});
        solver.addBody(Body<T, 2>(planet.mass, position, velocity));
    }

    std::ofstream outFile("../particle_positions_openmp.txt");
    if (!outFile)
    {
        std::cerr << "Failed to open output file" << std::endl;
        return 1;
    }

    std::ofstream entropyFile("../entropy_log_openmp.txt");
    if (!entropyFile)
    {
        std::cerr << "Failed to open entropy file" << std::endl;
        return 1;
    }
    entropyFile << "# Step\tEntropy\n";

    const int numParticles = planets.size();
    outFile << "# Format: ParticleID\tX\tY\tE\n";
    outFile << "# Number of particles: " << numParticles << "\n\n";

    // const int numSteps = 1000000;
    const int numSteps = 10000;  // when test using small number of steps
    std::cout << "Starting simulation with " << numParticles << " particles for " << numSteps << " steps\n";
    auto start = std::chrono::high_resolution_clock::now();

    solver.calculateEnergy();
    writePositionsToFile(solver.getBodies(), 0, outFile);

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

        updateEntropyGrid(solver.getBodies(), entropyGrid, minX, maxX, minY, maxY, invDx, invDy);
        sampleCount += solver.getBodies().size();

        if (step % 1000 == 0)
        {
            solver.calculateEnergy();
            writePositionsToFile(solver.getBodies(), step, outFile);
            std::cout << "Completed step " << step << "/" << numSteps << "\n";

            double entropy = computeNormalizedEntropy(entropyGrid, sampleCount);
            entropyFile << step << "\t" << entropy << "\n";
        }
    }

    outFile.close();
    entropyFile.close();

    std::cout << "Simulation complete. Results written to particle_positions_openmp.txt\n";

    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();

    int threads = omp_get_max_threads();  // Number of OpenMP threads
    int body_count = planets.size();

    // Append to CSV file
    std::ifstream checkExist("../openmp_runtime_different_threads.csv");
    bool file_exists = checkExist.good();
    checkExist.close();

    std::ofstream csvFile("../openmp_runtime_different_threads.csv", std::ios::app);
    if (csvFile.is_open()) {
        if (!file_exists) {
            csvFile << "Bodies,Threads,TimePerStep\n";  // header
        }
        double time_per_step = duration / numSteps;
        csvFile << body_count << "," << threads << "," << time_per_step << "\n";
        csvFile.close();
    } else {
        std::cerr << "Failed to open CSV file.\n";
    }

    std::cout << "Total simulation time: " << duration << " seconds using " << threads << " OpenMP threads.\n";
    return 0;
}
