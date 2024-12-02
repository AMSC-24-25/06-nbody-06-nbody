#include <NbodySolver.hpp>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    //1D SOLVER
    const double deltaT = 1e-5;
    const double numBodies = 2;
    const double T = 10;
    const unsigned int outputFreq = 1;

    NbodySolver solver(numBodies,deltaT);

    //Add bodies
    Vector<double,2> pos1({0.0,0.0});
    Vector<double,2> vel1({0.0,0.0});
    solver.addBody(5.970e24,pos1,vel1);

    Vector<double,2> pos2({1e8,0.0});
    Vector<double,2> vel2({0.0,1.996e3});
    solver.addBody(1,pos2,vel2);

    //Proceed with the simulation
    unsigned int timestepCounter = 0;
    for(double t=0;t<T;t+=deltaT){
        solver.step();
        if(timestepCounter%outputFreq==0){
            solver.output();
        }
        timestepCounter++;
    }
    
    return 0;
}