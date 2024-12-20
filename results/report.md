# Numerical N-Body Solution
## Introduction
In an n-body problem, we need to find the positions and velocities of a collection of
interacting particles over a period of time. For example, an astrophysicist might want
to know the positions and velocities of a collection of stars, while a chemist might
want to know the positions and velocities of a collection of molecules or atoms. An
n-body solver is a program that finds the solution to an n-body problem by simulating
the behavior of the particles.

## First formulation
We started our developing from an n-body solver that simulates the motions
of planets or stars.<br />
That is, the force excerted on particle $`q`$ by particle $`p`$ is given by
```math
\vec{f}_{q,p}(t)=-G\frac{m_qm_p}{\left|\vec{s}_q(t)-\vec{s}_p(t)\right|^3}\left[\vec{s}_q(t)-\vec{s}_p(t)\right]
```
where $`G`$ is the Gravitational constant ($`\approx 6.7\times 10^{−11}`$)<br/><br/>
The total force acting on a particle is therefore
```math
\vec{F}_q(t)=\sum_{i\neq q}^{N}\vec{f}_{q,i}(t)
```
From Newton's second law, we can compute the particles acceleration as
```math
\vec{a}_q(t) = \frac{\vec{F}_q(t)}{m_q}
```
Using a first-order integration scheme (in this case Forward Euler) we can compute the updated velocities and positions at time $`t+\Delta t`$
```math
\vec{s}_q(t+\Delta t)\approx \vec{s}_q(t)+\Delta t\vec{v}_q(t)
```
```math
\vec{v}_q(t+\Delta t)\approx \vec{v}_q(t)+\Delta t\vec{a}_q(t)
```

## Optimizations
### Computation of forces
Using Newton's third law, we can halve the amount of forces computed, since
```math
\vec{f}_{p,q}(t)=-\vec{f}_{q,p}(t)
```
### Better integration Scheme
The Forward Euler scheme, being first-order, performs bad in terms of accuracy when the $`\Delta t`$ is not extremely small. 
It's therefore crucial to choose a better integration scheme for the computation of the velocities and positions.<br />
As a first optimization, we used a Leapfrog KDK scheme [2], which is second order, and is composed as follows:
```math
\vec{v}_q(t+\frac{\Delta t}{2}) = \vec{v}_q(t)+\vec{a}_q(t)\frac{\Delta t}{2}
```
```math
\vec{s}_q(t+\Delta t) = \vec{s}_q(t) + \Delta t \vec{v}_q(t+\frac{\Delta t}{2})
```
```math
\vec{v}_q(t+\Delta t) = \vec{v}_q(t+\frac{\Delta t}{2})+\vec{a}_q(t+\Delta t)\frac{\Delta t}{2}
```

## Metric for measuring error
The total energy of the system is a widely used metric for assessing the accuracy of an n-body simulator [3]. In an idealized n-body system (e.g., one with no external forces or energy losses due to radiation), the total energy, which is the sum of the kinetic and potential energies of all bodies, should remain constant over time according to the principle of conservation of energy.
### Difference in $`\Delta E`$ for different choices of $`\Delta t`$
| Forward Euler  | Leapfrog |
| ------------- | ------------- |
| <img src="https://github.com/AMSC-24-25/06-nbody-06-nbody/blob/main/results/euler-order.png" alt="FE"> | <img src="https://github.com/AMSC-24-25/06-nbody-06-nbody/blob/main/results/leapfrog-order.png" alt="LEAPFROG"> |
### Comparison of integration schemes ($`\Delta t = 10^{-3}`$)

<img src="https://github.com/AMSC-24-25/06-nbody-06-nbody/blob/main/results/leap-euler.png" alt="scalability">
<img src="https://github.com/AMSC-24-25/06-nbody-06-nbody/blob/main/results/leap-euler-zoomed.png" alt="scalability">

The difference between the two methods in terms of $`\Delta E`$ have a significant effect on the accuracy of the simulation, as shown here ($`\Delta t=10^{-4},\ T=10`$, figure-8 configuration [1])
| Forward Euler  | Leapfrog |
| ------------- | ------------- |
| <img src="https://github.com/AMSC-24-25/06-nbody-06-nbody/blob/main/results/8euler-animation.gif" alt="FE"> | <img src="https://github.com/AMSC-24-25/06-nbody-06-nbody/blob/main/results/8leapfrog-animation.gif" alt="LEAPFROG"> |

## Parallel version
Parallelizing using OpenMP yeilds the following results in terms of scalability (# Bodies = 100, times in µs)
<img src="https://github.com/AMSC-24-25/06-nbody-06-nbody/blob/main/results/scalability.png" alt="scalability">

## 50 Bodies
<img src="https://github.com/AMSC-24-25/06-nbody-06-nbody/blob/main/results/20bodies.gif" alt="50bodies">

## References
[[1] Periodic Planar Three Body Orbits](https://observablehq.com/@rreusser/periodic-planar-three-body-orbits)<br/>
[[2] Leapfrog (Wikipedia)](https://en.m.wikipedia.org/wiki/Leapfrog_integration#)<br/>
[[3] The kinds of errors made in N-body simulations](https://www.cs.toronto.edu/~wayne/research/thesis/msc/node5.html)<br/>
