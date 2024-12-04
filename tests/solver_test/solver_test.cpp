#include <NbodySolver.hpp>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    // 2d 3 bodies I.A. i.c. 1 from https://numericaltank.sjtu.edu.cn/three-body/three-body-movies.htm
    const double deltaT = 1e-4;
    const double numBodies = 3;
    const double T = 5;
    const unsigned int outputFreq = 1000;

    NbodySolver solver(numBodies, deltaT);


    // Add bodies
    solver.addBody(1.0, Vector2d({0.6661637520772179,-0.081921852656887}), Vector2d({0.84120297540307,0.029746212757039}));
    solver.addBody(1.0, Vector2d({-0.025192663684493022,0.45444857588251897}), Vector2d({0.142642469612081,-0.492315648524683}));
    solver.addBody(1.0, Vector2d({-0.10301329374224,-0.765806200083609}), Vector2d({-0.98384544501151,0.462569435774018}));

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