#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>

int main(int argc, char* argv[]) {
    // Read number of bodies from first argument, default to 50
    size_t N = (argc > 1) ? std::stoi(argv[1]) : 50;

    // Read output filename from second argument, default to "generated_bodies.txt"
    std::string output_file = (argc > 2) ? argv[2] : "generated_bodies.txt";

    // std::ofstream out(output_file);
    std::string full_path = "GeneratedBodies/" + output_file;
    std::ofstream out(full_path);
    
    if (!out) {
        std::cerr << "Failed to open output file: " << output_file << std::endl;
        return 1;
    }

    std::srand(std::time(nullptr));

    for (size_t i = 0; i < N; ++i) {
        double x = ((double)rand() / RAND_MAX) - 0.5;
        double y = ((double)rand() / RAND_MAX) - 0.5;
        double vx = (((double)rand() / RAND_MAX) - 0.5) * 0.1;
        double vy = (((double)rand() / RAND_MAX) - 0.5) * 0.1;
        double m = ((double)rand() / RAND_MAX) * 100.0 + 1.0;

        out << x << " " << y << " " << vx << " " << vy << " " << m << "\n";
    }

    std::cout << "Generated " << N << " bodies to file: " << output_file << std::endl;
    return 0;
}