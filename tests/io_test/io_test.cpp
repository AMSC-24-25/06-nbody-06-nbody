#include "IO.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iomanip>

void testReadBodies()
{
    std::ofstream tempInput("testInput.txt");
    tempInput << "1.0 1.0 2.0 2.0 -3.0\n"; // Body2d 1
    tempInput << "4.0 3.0 -1.0 6.0 4.0\n"; // Body2d 2
    tempInput.close();

    // Read bodies using IO class
    std::vector<Body2d> bodies = IO::readBodiesFromFile("testInput.txt");

    // Detailed checks
    assert(bodies.size() == 2);

    // First body checks
    assert(bodies[0].getMass() == 1.0);
    assert(bodies[0].getPosition()[0] == 1.0);
    assert(bodies[0].getPosition()[1] == 2.0);
    assert(bodies[0].getVelocity()[0] == 2.0);
    assert(bodies[0].getVelocity()[1] == -3.0);

    // Second body checks
    assert(bodies[1].getMass() == 4.0);
    assert(bodies[1].getPosition()[0] == 3.0);
    assert(bodies[1].getPosition()[1] == -1.0);
    assert(bodies[1].getVelocity()[0] == 6.0);
    assert(bodies[1].getVelocity()[1] == 4.0);

    std::cout << "Test read bodies works." << std::endl;
    std::remove("testInput.txt");
}

void testWriteBodies()
{
    std::vector<Body2d> bodies = {
        Body2d(1.0, {1.0, 2.0}, {2.0, -3.0}),
        Body2d(4.0, {3.0, -1.0}, {6.0, 4.0})};

    IO::writeBodiesToFile("testOutput.txt", bodies);

    // More robust file reading
    std::ifstream tempOutput("testOutput.txt");
    std::string line;
    std::vector<std::string> lines;

    while (std::getline(tempOutput, line))
    {
        if (!line.empty())
        {
            lines.push_back(line);
        }
    }

    // Check number of lines
    assert(lines.size() == 2);

    // Direct string comparison
    assert(lines[0] == "1 1 2 2 -3");
    assert(lines[1] == "4 3 -1 6 4");

    std::cout << "Test write bodies works." << std::endl;

    std::remove("testOutput.txt");
}

int main()
{
    testReadBodies();
    testWriteBodies();

    std::cout << "IO test works." << std::endl;
    return 0;
}