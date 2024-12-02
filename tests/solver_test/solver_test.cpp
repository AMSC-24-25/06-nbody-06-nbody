#include <NbodySolver.hpp>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    //1D SOLVER
    const double deltaT = 1e-5;
    const double numBodies = 2;
    const double T = 1;
    const unsigned int outputFreq = 1;

    NbodySolver solver(numBodies,deltaT);

    //Add bodies
    Vector<double,1> v({1.0});
    for(int i=0;i<numBodies;i++){
        solver.addBody(1.0,i*v,i*v);
    }

    //Proceed with the simulation
    unsigned int timestepCounter = 0;
    for(double t=0;t<T;t+=deltaT){
        solver.step();
        if(timestepCounter%outputFreq==0){
            cout << "Timestep: " << timestepCounter << endl;
            solver.output();
            cout << endl;
        }
        timestepCounter++;
    }
    
    return 0;
}