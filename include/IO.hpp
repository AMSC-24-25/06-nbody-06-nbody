#ifndef IO_HPP
#define IO_HPP

#include "Body.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class IO
{
public:
    // Read body data from a file
    static std::vector<Body2d> readBodiesFromFile(const std::string &filename)
    {
        std::ifstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return {};
        }

        std::vector<Body2d> bodies;
        std::string line;

        while (std::getline(file, line))
        {
            // Skip empty lines
            if (line.empty())
                continue;

            std::istringstream iss(line);
            double mass, x, y, vx, vy;

            // Try to read 5 values
            if (iss >> mass >> x >> y >> vx >> vy)
            {
                Vector2d position = {x, y};
                Vector2d velocity = {vx, vy};

                bodies.emplace_back(mass, position, velocity);
            }
            else
            {
                std::cerr << "Error parsing line: " << line << std::endl;
            }
        }

        file.close();
        return bodies;
    }

    // Write body data to a file
    static void writeBodiesToFile(const std::string &filename, const std::vector<Body2d> &bodies)
    {
        std::ofstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }

        for (const auto &body : bodies)
        {
            file << static_cast<int>(body.getMass()) << " "
                 << static_cast<int>(body.getPosition()[0]) << " "
                 << static_cast<int>(body.getPosition()[1]) << " "
                 << static_cast<int>(body.getVelocity()[0]) << " "
                 << static_cast<int>(body.getVelocity()[1]) << "\n";
        }
        file.close();
    }
};

#endif