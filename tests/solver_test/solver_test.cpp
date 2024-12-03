#include <NbodySolver.hpp>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    // 2d 3 bodies (known configuration)
    const double deltaT = 1e-2;
    const double numBodies = 3;
    const double T = 1;
    const unsigned int outputFreq = 1;

    NbodySolver solver(numBodies, deltaT);

    // Add bodies
    solver.addBody(1.0, Vector2d({-2.0, 0.0}), Vector2d({0.3672, 0.5635}));
    solver.addBody(1.0, Vector2d({0.0, 0.0}), Vector2d({-0.3672, -0.5635}));
    solver.addBody(1.0, Vector2d({2.0, 0.0}), Vector2d({0.3672, 0.5635}));

    // Proceed with the simulation
    unsigned int timestepCounter = 0;
    for (double t = 0; t < T; t += deltaT)
    {
        solver.step();
        if (timestepCounter % outputFreq == 0)
        {
            solver.output();
        }
        timestepCounter++;
    }

    return 0;
}