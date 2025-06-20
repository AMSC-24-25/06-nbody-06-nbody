import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("../build/fmm_accuracy_vs_fault_tolerance.txt", skiprows=1)
eps = data[:, 0]
rel_tot = data[:, 2]
rel_max = data[:, 4]

plt.figure(figsize=(8, 5))
plt.plot(eps, rel_tot, 'o-', label='Relative Total Error ($E_{rel,tot}$)')
plt.plot(eps, rel_max, 's-', label='Relative Max Error ($E_{rel,max}$)')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('fault_tolerance_eps')
plt.ylabel('Relative Error')
plt.title('FMM Relative Error vs fault_tolerance_eps')
plt.legend()
plt.grid(True, which='both', ls='--', alpha=0.5)
plt.tight_layout()
plt.savefig("fmm_error_vs_fault_tolerance.png", dpi=150)
