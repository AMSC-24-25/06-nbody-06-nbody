
# 🌌 Barnes-Hut method using OpenMP

This branch contains a **multithreaded CPU implementation** of the N-Body simulation using OpenMP with the Barnes-Hut method.

## 🧠 Description

- Organizes space into a tree (e.g., quad-tree in 2D, octree in 3D) for grouping distant particles. This branch uses a 2D quad-tree implementation.
- Distant particle groups are treated as single "pseudo-particles" using their center of mass.

## 🧪 How to Run

### 🔧 Prerequisites

- A C++ compiler with OpenMP support (e.g., `g++` or `clang++`)

### 🛠️ Compilation

There are various already-made code snippets in the `tests` folder.  
To build and run opemmp version, go in the appropriate folder and run the following:

```bash
# 1. Generate the initial condition file (e.g., for 5000 bodies):
g++ GenerateBodies.cpp -o generate_bodies
./generate_bodies 5000 generated_bodies_5000.txt

# 2. Compile and run the OpenMP-based simulation:
g++-14 -fopenmp -O2 BH_test_openmp.cpp -I../../include -o bh_openmp
OMP_NUM_THREADS=1 ./bh_openmp
OMP_NUM_THREADS=2 ./bh_openmp
OMP_NUM_THREADS=4 ./bh_openmp
OMP_NUM_THREADS=8 ./bh_openmp
OMP_NUM_THREADS=16 ./bh_openmp

# 3. Plot runtime performance using the generated CSV:
python3 Plot_openmp_threads.py
```

To make your own tests, modify the file accordingly. The current CMakeLists.txt is set up to compile the serial barnes-hut implementation only, here iare demands to run:

```bash
mkdir build
cd build
cmake ..
make
./bh_test
```

### 📄 Input File Format

The input file for initial positions should contain:

```
N
m1
x1 y1
vx1 vy1
m2
x2 y2
vx2 vy2
...
mN
xN yN
vxN vyN
```

Where:

- `N` is the number of particles  
- `mK` is the k-th body mass  
- `xK yK` is the k-th body initial position  
- `vxK vyK` is the k-th body initial velocity

### 🔎 Output

The program can output:

- Position of each body per timestep  
- Total energy per timestep
- CSV summary files for plotting performance metrics
