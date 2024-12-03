#ifndef IO_HPP
#define IO_HPP

#include "Body.hpp"
#include <vector>
#include <string>
#include <fstream>

class IO {
public:
    // Read body data from a file
    static std::vector<Body2d> readBodiesFromFile(const std::string& filename) {
        
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
        }

        std::vector<Body2d> bodies;
        double mass, x, y, vx, vy;
        while (file >> mass >> x >> y >> vx >> vy) {
            bodies.emplace_back(mass, std::vector<double>{x, y}, std::vector<double>{vx, vy});

        }
        file.close();
        return bodies;
    }

    // Write body data to a file
    static void writeBodiesToFile(const std::string& filename, const std::vector<Body2d>& bodies) {
        std::ofstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }

        for (const auto& body : bodies) {
            file << body.getMass() << " "
                 << body.getPosition()[0] << " " << body.getPosition()[1] << " "
                 << body.getVelocity()[0] << " " << body.getVelocity()[1] << "\n";
        }
        file.close();
    }
};

#endif 