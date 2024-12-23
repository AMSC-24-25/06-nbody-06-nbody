#include <NbodySolver.hpp>
#include <chrono>

using namespace std;

int main(int argc, char **argv)
{
    const Real deltaT = 1e-4;
    const Real T = 30;
    const unsigned int outputFreq = 100;

    NbodySolver solver;

    // Load bodies
    solver.loadBodies("input.txt");

    // Open output file (which is kept open till the end to reduce overhead)
    ofstream outFile("bodyData.txt");
    if (!outFile.is_open())
    {
        cerr << "Error: Cannot open output file " << endl;
        return -1;
    }

    // Print bodies information
    solver.outputData(outFile);
    // Run solver
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    chrono::steady_clock::time_point end;
    unsigned int timestepCounter = 0;
    #pragma omp parallel
    {
        #pragma omp single
        solver.initSharedVar();
        for (Real t = 0; t < T; t += deltaT)
        {
            solver.step(deltaT);
            #pragma omp single
            {
                if (timestepCounter % outputFreq == 0)
                {
                    solver.outputTimestep(outFile);
                    cout << "Timestep: " << t << endl;
                }
                timestepCounter++;
            }
        }
    }

    outFile.close();
    return 0;
}