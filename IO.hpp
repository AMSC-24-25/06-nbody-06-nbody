#ifndef IO_HPP
#define IO_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "vector.hpp"
#include "body.hpp"

// print out on screen
template <typename T, int N>
void print_bodies(const std::vector<Body<T, N>>& bodies)
{
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        const auto& b = bodies[i];
        std::cout << "Body " << i << ": Final Pos=" << b.getPosition()
            << ", Vel=" << b.getVelocity()
            << ", Acc=" << b.getAcceleration() << "\n";
    }
}

// write back to file
template <typename T, int N>
void write_trajectory_csv_header(std::ofstream& fout)
{
    fout << "step,id";
    for (int d = 0; d < N; ++d)
        fout << ",x" << d;
    fout << "\n";
}


template <typename T, int N>
void write_trajectory_csv_frame(std::ofstream& fout, const std::vector<Body<T, N>>& bodies, int step)
{
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        fout << step << "," << i;
        const auto& pos = bodies[i].getPosition();
        for (int d = 0; d < N; ++d)
            fout << "," << pos[d];
        fout << "\n";
    }
}

// read from txt file
template <typename T, int N>
std::vector<Body<T, N>> read_bodies_from_txt(const std::string& filename)
{
    

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Error: Cannot open file " << filename <<std:: endl;
        return {};
    }


    int numBodies;
    file >> numBodies;
    if (!file) throw std::runtime_error("Invalid file: missing number of bodies");

    std::vector<Body<T, N>> bodies;
    for (int i = 0; i < numBodies; ++i)
    {
        T m, pos[N], vel[N];

        file >> m;
        if (!file) throw std::runtime_error("Invalid mass at particle " + std::to_string(i));

        // position line
        for (int j = 0; j < N; ++j)
            file >> pos[j];
        if (!file) throw std::runtime_error("Invalid position at particle " + std::to_string(i));

        // velocity line
        for (int j = 0; j < N; ++j)
            file >> vel[j];
        if (!file) throw std::runtime_error("Invalid velocity at particle " + std::to_string(i));

        Vector<T, N> p, v;
        for (int j = 0; j < N; ++j) {
            p[j] = pos[j];
            v[j] = vel[j];
        }


        bodies.emplace_back(m, p, v);
    }

    return bodies;
}

#endif
