#include <NbodySolver.hpp>
#include <chrono>
#include <stdlib.h>

static constexpr int nt = 8;

using namespace std;

int main(int argc, char **argv)
{
    srand(time(NULL));
    // Test 10 timesteps
    const Real deltaT = 1;
    const Real T = 10;
    const unsigned int outputFreq = 1;

    NbodySolver solver;

    // Add bodies
    for (int j = 0; j < 10; j++)
    {
        solver.addBody(
            1.0,
            {((double)rand() / RAND_MAX) * 10.0 - 5.0,
             ((double)rand() / RAND_MAX) * 10.0 - 5.0},
            {((double)rand() / RAND_MAX) * 10.0 - 5.0,
             ((double)rand() / RAND_MAX) * 10.0 - 5.0});
    }

    // Run solver
    chrono::steady_clock::time_point begin;
    chrono::steady_clock::time_point end;
    unsigned int timestepCounter = 0;
    solver.initSharedVar();
    for (int i = 0; i < 4; i++)
    {
        #pragma omp parallel num_threads(nt)
        {
            #pragma omp single
            begin = chrono::steady_clock::now();
            for (Real t = 0; t < T; t += deltaT)
            {
                solver.step(deltaT);
                #pragma omp single
                {
                    cout << omp_get_num_threads() << endl;
                    end = chrono::steady_clock::now();
                    // cout << i << " " << " " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << endl;
                    begin = chrono::steady_clock::now();
                }
                timestepCounter++;
            }
        }

        // Add bodies
        for (int j = 0; j < 9.0 * pow(10, i); j++)
        {
            solver.addBody(
                1.0,
                {((double)rand() / RAND_MAX) * 10.0 - 5.0,
                 ((double)rand() / RAND_MAX) * 10.0 - 5.0},
                {((double)rand() / RAND_MAX) * 10.0 - 5.0,
                 ((double)rand() / RAND_MAX) * 10.0 - 5.0});
        }
        timestepCounter = 0;
    }
    return 0;
}