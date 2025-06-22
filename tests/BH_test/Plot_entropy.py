import matplotlib.pyplot as plt
import sys

filename = sys.argv[1] if len(sys.argv) > 1 else "entropy_log.txt"

steps = []
entropy = []

# with open("entropy_log.txt") as f:
with open(filename) as f:
    for line in f:
        if line.startswith("#"):
            continue
        s, e = line.strip().split()
        steps.append(int(s))
        entropy.append(float(e))

plt.plot(steps, entropy, label="Shannon Entropy")
plt.xlabel("Simulation Step (with 1e-5 s per step)")
plt.ylabel("Entropy")
plt.title("Entropy Evolution in 3-Body Simulation")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("entropy_plot.png")

# Save with a dynamic name 
output_name = "entropy_plot.png"
plt.savefig(output_name)