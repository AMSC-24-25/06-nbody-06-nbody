## **Goal**

Based on the same algorithm (Leapfrog), we want to know how much it can accelerate if we use **CUDA** instead of the basic **CPU** version and **OpenMP** version.

---

## **Implementation**

### • Parallel
We use every thread to compute the entire acceleration of every particle.

### • Memory Management
- Particle info is written to **shared memory** (`Body<Real, dim>* bodies`), which avoids frequent access to global memory.
- `cudaMemcpy` is used to copy data between Host and Device.

### • Tile Calculation
- Shared memory is used to ensure all tiles collaborate (but performance improvement is limited).

---

## **The Energy Stability of CUDA Version**

![Image from Google Drive](https://drive.google.com/uc?export=view&id=1-5S1qt4WS4UZPgrP3HfeLqwucbCD8C0k)

---

## **Benchmark**


| Condition(delta t=0.001 T=1) | CUDA | OpenMP | Basic Version |
|----------|------|--------|---------------|
| input_size = 100 | 0.327 ms | 0.295 ms | 1.128 ms |
| input_size = 200 | 0.557 ms | 1.078 ms | 4.648ms |
| input_size = 500 | 0.719ms | 5.743 ms | 26.95 ms |
| input_size = 1000 | 1.773 ms | 21.82 ms | 115.329 ms |

> **Table:** Time Cost Per Step  — CUDA vs OpenMP vs Basic Version
---
Based on the cuda run time is not very stable, we run the program several times and calculate the average value.
We can see from the table that when the number of particles is large, the advantage of CUDA is obvious.
## **Possible Ways to Improve**

1. Merge possible kernels  
2. Use `float4` instead of `double`  
3. Apply **loop unrolling** techniques  
4. Use CUDA intrinsic functions like `__fsqrt_rn(x)` instead of `sqrt(x)`  
5. Find proper **tile size** for large number of particles  

---

## **Reference**

[NVIDIA GPU Gems 3 — Chapter 31: Fast N-Body Simulation with CUDA](https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-31-fast-n-body-simulation-cuda)
