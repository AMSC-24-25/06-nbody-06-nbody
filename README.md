
# 🌌 Naive Method using OpenMP

This branch contains a **multithreaded CPU implementation** of the N-Body simulation using OpenMP with the Naive method.

## 🧠 Description

- Computes all the pairwise energies  
- Parallelizes force calculations across multiple CPU threads.

## 🧪 How to Run

### 🔧 Prerequisites

- A C++ compiler with OpenMP support (e.g., `g++` or `clang++`)

### 🛠️ Compilation

There are various already-made code snippets in the `tests` folder.  
To build them, go in the appropriate folder and run the following:

```bash
mkdir build
cd build
cmake ..
make
./<executable name>
```

To make your own tests, modify the *CMakeListsTemplate.txt* file accordingly.

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

Some examples for initial conditions can be found in the folder *initial_conditions*.

### 🔎 Output

The program can output:
- Position of each body per timestep  
- Total energy per timestep
