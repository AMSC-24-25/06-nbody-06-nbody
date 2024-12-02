#include<iostream>
#include<vector>
#include<cmath>
#include <Eigen/Sparse>
using namespace std;
struct  particle
{
    double mass;
    Eigen::Vector3d position;
    Eigen::Vector3d velocity;
};

const double G = 6.67e-11; // gravity constant
const double dt = 0.01;       // time diff

Eigen::Vector3d computeForce(const particle& a, const particle& b) 
{
    Eigen::Vector3d r = b.position-a.position; // vector distance
    double distance = r.norm(); // scalar distance
    if (distance == 0) 
        return Eigen::Vector3d(0, 0, 0); 
    double forceMagnitude = G * a.mass * b.mass / (distance * distance); // force ofGENE gravity
    return r * (forceMagnitude / distance); 
}

// update the state
void updateparticle(particle Particles[])
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
    particle particle1,particle2,particle3;
    particle1={1,Eigen::Vector3d (0, 0, 1),Eigen::Vector3d (0.3, 0, 1)};
    particle2={2,Eigen::Vector3d (1, 1.5, 1),Eigen::Vector3d (1, 0, 1)};
    particle3={3,Eigen::Vector3d (0, 2, 1),Eigen::Vector3d (0.3, 0.5, 1)};

    particle bodies[3] = {particle1, particle2, particle3};//assume there are 3 bodies

    // simulation
    for (int time = 0; time < 10000; ++time) {
        updateparticle(bodies);

        // print the states 
        if (time % 100 == 0) 
        {
            std::cout << "Step " << step << ":\n";
            for (size_t i = 0; i < 3; ++i) {
                std::cout << "  particle " << i + 1 << " Position: ("
                          << bodies[i].position[0] << ", "
                          << bodies[i].position[1] << ", "
                          << bodies[i].position[2] << ")\n";
            }
             }
    }

    return 0;
}