import numpy as np
import matplotlib.pyplot as plt

# --- Load error data ---
data = np.loadtxt("../build/accuracy_error_vs_time.txt", skiprows=1)
# Columns: t, E_abs_tot, E_rel_tot, E_abs_max, E_rel_max

time = data[:, 0]
rel_tot = data[:, 2]
rel_max = data[:, 4]

plt.figure(figsize=(8, 5))
plt.plot(time, rel_tot, label="Relative Total Error ($E_{rel,tot}$)")
plt.plot(time, rel_max, label="Relative Max Error ($E_{rel,max}$)")
plt.yscale("log")
plt.xlabel("Time")
plt.ylabel("Relative Error")
plt.title("FMM vs Direct: Relative Errors Over Time")
plt.legend()
plt.grid(True, which="both", ls="--", alpha=0.5)
plt.tight_layout()
plt.savefig("accuracy_error_vs_time.png", dpi=150)
plt.show()
