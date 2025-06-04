import matplotlib.pyplot as plt

steps = []
entropy = []

with open("entropy_log.txt") as f:
    for line in f:
        if line.startswith("#"):
            continue
        s, e = line.strip().split()
        steps.append(int(s))
        entropy.append(float(e))

plt.plot(steps, entropy, label="Shannon Entropy")
plt.xlabel("Simulation Step")
plt.ylabel("Entropy")
plt.title("Entropy Evolution in 3-Body Simulation")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("entropy_plot.png")
