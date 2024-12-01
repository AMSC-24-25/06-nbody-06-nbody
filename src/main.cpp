#include "Visualizer.hpp"

int main()
{
    Visualizer visualizer(1280, 720, "nbody");

    if (visualizer.init()) {
        visualizer.run();
    }

    return 0;
}
