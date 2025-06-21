import numpy as np
import matplotlib.pyplot as plt

filename = "../build/integration_efficiency.txt"

# --- Read the mode ---
with open(filename, 'r') as f:
    first_line = f.readline()
    while first_line.strip() == '' or not first_line.startswith("# mode:"):
        first_line = f.readline()
    mode = first_line.strip().split(":")[1].strip()
    while True:
        header = f.readline()
        if not header:
            raise RuntimeError("File ended before header found!")
        if header.startswith("#"):
            columns = header.strip().strip("#").split()
            break

# --- Load the data ---
data = np.loadtxt(filename, comments='#')
col_idx = {col: i for i, col in enumerate(columns)}
avg_time_ms = data[:, col_idx['avg_time_per_step_ms']]

plt.figure(figsize=(8, 5))

if mode == "N":
    x = data[:, col_idx['N']]
    xlabel = 'Number of Particles $N$'
    plot_title = 'Average FMM Step Time vs. $N$'
    plt.xscale('log')
    plt.yscale('log')
elif mode == "items_per_leaf":
    x = data[:, col_idx['items_per_leaf']]
    xlabel = 'Items per Leaf'
    plot_title = 'Average FMM Step Time vs. Items per Leaf'
elif mode == "eps":
    x = data[:, col_idx['eps']]
    xlabel = 'Fault Tolerance $\epsilon$'
    plot_title = 'Average FMM Step Time vs. Fault Tolerance'
    plt.xscale('log')
    plt.gca().invert_xaxis()
else:
    raise ValueError("Unknown mode: {}".format(mode))

plt.plot(x, avg_time_ms, 'o-', label='Average Time per Step (ms)')

if mode == "N":
    # O(N) reference curve
    cN = avg_time_ms[0] / x[0]
    O_N = cN * x
    plt.plot(x, O_N, '--', color='gray', label='$\mathcal{O}(N)$ reference')

    # O(N log N) reference curve
    cNlogN = avg_time_ms[0] / (x[0] * np.log2(x[0]))
    O_NlogN = cNlogN * x * np.log2(x)
    plt.plot(x, O_NlogN, '--', color='red', label='$\mathcal{O}(N \log N)$ reference')

plt.xlabel(xlabel, fontsize=14)
plt.ylabel('Time per Step (ms)', fontsize=14)
plt.title(plot_title, fontsize=15)
plt.grid(True, which='both', ls='--', alpha=0.6)
plt.legend()
plt.tight_layout()
plt.savefig(f"integration_efficiency_{mode}.png", dpi=150)
