import numpy as np
import matplotlib.pyplot as plt

# Read the data (skip comment/header lines)
data = np.loadtxt("../build/integration_efficiency.txt", comments='#')
N = data[:, 0]
total_time_ms = data[:, 1]
avg_time_ms = data[:, 2]

plt.figure(figsize=(8, 5))

# Plot your actual measured times
plt.plot(N, total_time_ms, 'o-', label='Total Time (ms)')
plt.plot(N, avg_time_ms, 's-', label='Average Time per Step (ms)')

plt.xscale('log')
plt.yscale('log')
plt.xlabel('Number of Particles $N$', fontsize=14)
plt.ylabel('Time (ms)', fontsize=14)
plt.title('FMM Integration Efficiency vs. Number of Particles', fontsize=15)
plt.grid(True, which='both', ls='--', alpha=0.6)

# Set x-ticks for exact N values (integers)
plt.xticks(N, [str(int(n)) for n in N])

# --- O(N) reference ---
cN = total_time_ms[0] / N[0]
O_N = cN * N
plt.plot(N, O_N, '--', color='gray', label='$\mathcal{O}(N)$ reference')

# --- O(N log N) reference ---
cNlogN = total_time_ms[0] / (N[0] * np.log2(N[0]))
O_NlogN = cNlogN * N * np.log2(N)
plt.plot(N, O_NlogN, '--', color='red', label='$\mathcal{O}(N \log N)$ reference')

plt.legend()
plt.tight_layout()
plt.savefig("integration_efficiency_scaling.png", dpi=150)

