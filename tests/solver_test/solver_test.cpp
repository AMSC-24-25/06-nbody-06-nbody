#include <NbodySolver.hpp>

using namespace std;

int main(int argc, char **argv)
{
    // 2d 3 bodies I.A. i.c. 1 from https://numericaltank.sjtu.edu.cn/three-body/three-body-movies.htm
    const Real deltaT = 1e-6;
    const Real T = 5;
    const unsigned int outputFreq = 10000;

    NbodySolver solver;

    // Load bodies
    solver.loadBodies("input.txt");

    // Proceed with the simulation
    // Open output file (which is kept open till the end to reduce overhead)
    ofstream outFile("output.txt");
    if (!outFile.is_open())
    {
        cerr << "Error: Cannot open output file " << endl;
        return -1;
    }

    // Print bodies information
    solver.outputData(outFile);
    // Run timesteps
    unsigned int timestepCounter = 0;
    for (Real t = 0; t < T; t += deltaT)
    {
        solver.step(deltaT);
        if (timestepCounter % outputFreq == 0)
        {
            // Output timestep data
            solver.outputTimestep(outFile);
            cout << "Time: " << t << endl;
        }
        timestepCounter++;
    }

    outFile.close();
    return 0;
}