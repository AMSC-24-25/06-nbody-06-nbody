import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("../build/fmm_accuracy_vs_items_per_leaf.txt", skiprows=1)
ipl = data[:, 0]
rel_tot = data[:, 2]
rel_max = data[:, 4]

plt.figure(figsize=(8, 5))
plt.plot(ipl, rel_tot, 'o-', label='Relative Total Error')
plt.plot(ipl, rel_max, 's-', label='Relative Max Error')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Items Per Leaf')
plt.ylabel('Relative Error')
plt.title('FMM Error vs Items Per Leaf')
plt.legend()
plt.grid(True, which='both', ls='--', alpha=0.6)
plt.tight_layout()

# Set xticks to actual unique values, formatted as ints
plt.xticks(ipl, [str(int(x)) for x in ipl])

plt.savefig("fmm_error_vs_items_per_leaf.png", dpi=150)
plt.show()
