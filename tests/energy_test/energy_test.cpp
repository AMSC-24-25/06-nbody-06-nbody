#include <NbodySolver.hpp>

using namespace std;

int main(int argc, char **argv)
{
    const Real deltaT = 1e-4;
    const Real T = 50;
    const unsigned int outputFreq = 1;

    NbodySolver solver;

    // Load bodies
    solver.loadBodies("input.txt");

    // Open output file (which is kept open till the end to reduce overhead)
    ofstream outFile("energyData.txt");
    if (!outFile.is_open())
    {
        cerr << "Error: Cannot open output file " << endl;
        return -1;
    }

    // Run solver
    Real energy = solver.computeEnergy();
    unsigned int timestepCounter = 0;
    // Initial energy
    outFile << "-1 " << energy << endl;
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
                    cout << t << endl;
                    // Compute energy
                    energy = solver.computeEnergy();
                    outFile << timestepCounter << " " << energy << endl;
                }

                timestepCounter++;
            }
        }
    }

    outFile.close();
    return 0;
}