#include "IO.hpp"

#include <iostream>
#include <fstream>
#include <cassert>

void testReadBodies() {
    std::ofstream tempInput("testInput.txt");
    tempInput << "1.0 1.0 2.0 2.0 -3.0 3.0\n";  // Body2d 1
    tempInput << "4.0 3.0 -1.0 6.0 4.0 4.0\n";  // Body2d 2

    // Read bodies using IO class
    std::vector<Body2d> bodies = IO::readBodiesFromFile("testInput.txt");

    // Check 
    assert(bodies.size() == 2);
    assert(bodies[0].getMass() == 1.0);
    assert(bodies[1].getPosition()[1] == -1.0);

    std::cout << "Test read bodies works." << std::endl;
    std::remove("testInput.txt");

}

void testWriteBodies() {
    std::vector<Body2d> bodies = {
        Body2d(1.0, {1.0, 2.0}, {2.0, -3.0}),
        Body2d(4.0, {3.0, -1.0}, {6.0, 4.0})
    };

    IO::writeBodiesToFile("testOutput.txt", bodies);

    //Check each row
    std::ifstream tempOutput("testOutput.txt");
    std::string row;

    std::getline(tempOutput, row);
    assert(row == "1.0 1.0 2.0 2.0 -3.0");
    std::getline(tempOutput, row);
    assert(row == "4.0 3.0 -1.0 6.0 4.0");

    std::cout << "test write bodies works." << std::endl;

    std::remove("testOutput.txt");
    
}

int main() {
    testReadBodies();
    testWriteBodies();

    std::cout << "IO test works." << std::endl;
    return 0;
}

