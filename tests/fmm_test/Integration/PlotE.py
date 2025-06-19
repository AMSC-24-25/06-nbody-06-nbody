import numpy as np
import matplotlib.pyplot as plt

# Read the data file
data = np.loadtxt('../build/energy_vs_time.txt', skiprows=1)  # skips header

t = data[:, 0]
E_kin = data[:, 1]
E_pot = data[:, 2]
E_tot = data[:, 3]

plt.figure(figsize=(8, 5))
plt.plot(t, E_kin, label="Kinetic Energy", color='tab:blue')
plt.plot(t, E_pot, label="Potential Energy", color='tab:orange')
plt.plot(t, E_tot, label="Total Energy", color='tab:green', linewidth=2)

plt.xlabel('Time')
plt.ylabel('Energy')
plt.title('Energy vs Time')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('energy_vs_time.png', dpi=120)
plt.show()
