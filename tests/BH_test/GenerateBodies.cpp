#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[]) {
    size_t N = (argc > 1) ? std::stoi(argv[1]) : 50;
    std::ofstream out("generated_bodies.txt");
    std::srand(std::time(nullptr));

    for (size_t i = 0; i < N; ++i) {
        double x = ((double)rand() / RAND_MAX) - 0.5;
        double y = ((double)rand() / RAND_MAX) - 0.5;
        double vx = ((double)rand() / RAND_MAX - 0.5) * 0.1;
        double vy = ((double)rand() / RAND_MAX - 0.5) * 0.1;
        double m = ((double)rand() / RAND_MAX) * 100.0 + 1.0;

        out << x << " " << y << " " << vx << " " << vy << " " << m << "\n";
    }

    out.close();
    std::cout << "Generated " << N << " bodies in generated_bodies.txt\n";
    return 0;
}