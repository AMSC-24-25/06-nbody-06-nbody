#include <NbodySolver.hpp>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    // 2d 3 bodies I.A. i.c. 1 from https://numericaltank.sjtu.edu.cn/three-body/three-body-movies.htm
    const double deltaT = 1e-5;
    const double numBodies = 3;
    const double T = 10;
    const unsigned int outputFreq = 1000;

    NbodySolver solver(numBodies, deltaT);

    // Add bodies
    solver.addBody(1.0, Vector2d({-1.0, 0.0}), Vector2d({0.3471168881, 0.5327249454}));
    solver.addBody(1.0, Vector2d({1.0, 0.0}), Vector2d({0.3471168881, 0.5327249454}));
    solver.addBody(1.0, Vector2d({0.0, 0.0}), Vector2d({-2*0.3471168881, -2*0.5327249454}));

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