#include<iostream>
#include<vector>
#include<cmath>
#include <Eigen/Sparse>
using namespace std;
struct  body
{
    double mass;
    double echarge;
    Eigen::Vector3d position;
    Eigen::Vector3d velocity;
};

const double k_e = 8.99e9; //Coulomb constant

const double G = 6.67e-11; // gravity constant
const double e_mass = 9.10938356e-31; 
const double dt = 0.01;       // time diff

Eigen::Vector3d computeForce(const body& a, const body& b) 
{
    Eigen::Vector3d r = b.position-a.position; // vector distance
    double distance = r.norm(); // scalar distance
    if (distance == 0) 
        return Eigen::Vector3d(0, 0, 0); 
    double forceMagnitude = G * a.mass * b.mass ; // force of gravity
    double eforce = k_e * a.echarge * b.echarge;//force of Coulomb
    return r * ((forceMagnitude+eforce) /distance * distance*distance); 
}

// update the state
void updateparticle(body Particles[])
 {
    Eigen::Vector3d forces[3];
    // update force
    for (size_t i = 0; i < 3; ++i) 
    {
        for (size_t j = 0; j < 3; ++j) {
            if (i != j) //inner not considered
            {
                forces[i] = forces[i] + computeForce(Particles[i], Particles[j]);
            }
        }
    }
    // update volocity & position
    for (size_t i = 0; i < 3; ++i) 
    {
        Eigen::Vector3d acceleration = forces[i]/ Particles[i].mass;
        Particles[i].velocity = Particles[i].velocity + acceleration * dt;
        Particles[i].position = Particles[i].position + Particles[i].velocity * dt;
    }
 }

int main()
{
    body body1,body2,body3;
    body1={170,1e-6,Eigen::Vector3d (200, 57, 1),Eigen::Vector3d (0.3, 0, 1)};
    body2={200,1e-6,Eigen::Vector3d (17, 1.5, 1),Eigen::Vector3d (1, 0, 1)};
    body3={320,1e-7,Eigen::Vector3d (0, 23, 1),Eigen::Vector3d (0.3, 0.5, 1)};

    body bodies[3] = {body1, body2, body3};//assume there are 3 bodies

    // simulation
    for (int time = 0; time < 1000; ++time) {
        updateparticle(bodies);

        // print the states 
        if (time % 100 == 0) 
        {
            std::cout << "Step " << time << ":"<<std::endl;
            for (size_t i = 0; i < 3; ++i) 
            {
                std::cout << "  body " << i + 1 << " Position: ("
                          << bodies[i].position[0] << ", "
                          << bodies[i].position[1] << ", "
                          << bodies[i].position[2] << ")"<<std::endl;
            }
             }
    }

    return 0;
}