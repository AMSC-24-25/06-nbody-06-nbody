#include <NbodySolver.hpp>
#include <chrono>

using namespace std;

int main(int argc, char **argv)
{
    // Try 10000 iterations to test speedup
    const Real deltaT = 1e-4;
    const Real T = 1e-2;
    const unsigned int outputFreq = 1;

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
    #pragma omp parallel num_threads(8)
    {
        for (Real t = 0; t < T; t += deltaT)
        {
            solver.step(deltaT);
            if (timestepCounter % outputFreq == 0)
            {
                #pragma omp single
                {
                    end = chrono::steady_clock::now();
                    cout << chrono::duration_cast<chrono::microseconds>(end - begin).count() << endl;
                    begin = chrono::steady_clock::now();
                }
            }
            timestepCounter++;
        }
    }

    outFile.close();
    return 0;
}