# N-Body Simulation

A collection of N-Body simulation implementations exploring different computational architectures and algorithmic methods. This project compares performance and accuracy across CPU and GPU platforms, providing a learning and benchmarking resource for high-performance computing applications.

## Project Overview

The N-Body problem simulates the motion of a system of particles under mutual gravitational influence. This repository includes several implementations with varying degrees of optimization, parallelism, and hardware utilization, enabling comparative analysis and performance study.

## Branches

The repository is organized into multiple branches, each containing a specific implementation:

- [`naive-openmp`](https://github.com/AMSC-24-25/06-nbody-06-nbody/tree/naive-openmp) - Basic OpenMP implementation using the Naive method. For reference.
- [`naive-cuda`](https://github.com/AMSC-24-25/06-nbody-06-nbody/tree/naive-cuda) - CUDA implementation using the Naive method.
- [`bh-openmp`](https://github.com/AMSC-24-25/06-nbody-06-nbody/tree/bh-openmp) - Both the serial and OpenMP verison implementation using the Barnes-Hut method.
- [**`bh-cuda`**](https://github.com/AMSC-24-25/06-nbody-06-nbody/tree/bh-cuda) - **Real-time CUDA implementation using the Barnes-Hut method and OpenGL rendering.**
- [**`fmm`**](https://github.com/AMSC-24-25/06-nbody-06-nbody/tree/FMM) - **Fast multipole method implementation with openmp parallelization.**

## Report

A detailed report discussing the design choices, performance evaluations, and results of the simulations can be found **[here](AMSC_Project_Report___Group_6.pdf)**.

## Authors

- Luca Ballone  
- Rong Huang  
- Andrea Barletta  
- Yanlong Wang  
- Zhaochen Qiao
