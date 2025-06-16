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

| Condition | CUDA | OpenMP | Basic Version |
|----------|------|--------|---------------|
| input_size = 100, T = 1000, ΔT = 0.01 | 853 ms | 30,380 ms | 110,024 ms |
| input_size = 100, T = 1000, ΔT = 0.1  | 732 ms | 3,126 ms  | 11,506 ms  |
| input_size = 200, T = 1000, ΔT = 0.01 | 1,014 ms | 108,657 ms | 458,277 ms |
| input_size = 200, T = 1000, ΔT = 0.1  | 2,169 ms | 11,170 ms  | 46,606 ms  |
| input_size = 4000, T = 1000, ΔT = 0.01| 34,240 ms | NA | NA |

> **Table:** Time Cost — CUDA vs OpenMP vs Basic Version

---

## **Possible Ways to Improve**

1. Merge possible kernels  
2. Use `float4` instead of `double`  
3. Apply **loop unrolling** techniques  
4. Use CUDA intrinsic functions like `__fsqrt_rn(x)` instead of `sqrt(x)`  
5. Find proper **tile size** for large number of particles  

---

## **Reference**

[NVIDIA GPU Gems 3 — Chapter 31: Fast N-Body Simulation with CUDA](https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-31-fast-n-body-simulation-cuda)
