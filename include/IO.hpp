#ifndef IO_HPP
#define IO_HPP

#include <Body.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

template <typename T, int dim>
class IO
{
private:
    ofstream output;

public:
    // Read body data from a file
    static vector<Body<T, dim>> readBodiesFromFile(const string &filename)
    {
        ifstream file(filename);

        if (!file.is_open())
        {
            cerr << "Error: Cannot open file " << filename << endl;
            return {};
        }

        vector<Body<T, dim>> bodies;
        string line;

        int numBodies;
        {
            getline(file, line);
            istringstream iss(line);
            iss >> numBodies;
        }
        for (int i = 0; i < numBodies; i++)
        {

            // Read mass
            massT mass;
            {
                getline(file, line);
                istringstream iss(line);
                // Read mass
                iss >> mass;
            }

            // Read position
            vector<T> pos;
            {
                getline(file, line);
                istringstream iss(line);

                for (int i = 0; i < dim; i++)
                {
                    T value;
                    iss >> value;
                    pos.emplace_back(value);
                }
            }

            // Read velocity
            vector<T> vel;
            {
                getline(file, line);
                istringstream iss(line);
                for (int i = 0; i < dim; i++)
                {
                    T value;
                    iss >> value;
                    vel.emplace_back(value);
                }
            }

            bodies.emplace_back(Body<T, dim>(mass, Vector<T, dim>(pos), Vector<T, dim>(vel)));
        }

        file.close();
        return bodies;
    }

    // Write simulation header
    static void writeBodiesInfo(ofstream &file, const vector<Body<T, dim>> &bodies)
    {
        file << bodies.size() << endl;

        for(const auto &body:bodies){
            file << body.getMass() << endl;
        }
    }

    // Write simulation data
    static void writeSimulationData(ofstream &file, const vector<Body<T, dim>> &bodies)
    {
        for (const auto &body : bodies)
        {
            file << body << endl;
        }
    }
};

#endif