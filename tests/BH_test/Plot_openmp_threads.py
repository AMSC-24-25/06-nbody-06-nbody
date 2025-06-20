import matplotlib.pyplot as plt
import pandas as pd

# Load the CSV 
df = pd.read_csv("../openmp_runtime_different_threads.csv")

# Filter for a specific body count (e.g. 500, 1000)
body_count = 500
df = df[df["Bodies"] == body_count]

# Sort by thread count to make the plot nice
df = df.sort_values("Threads")

# Plot: Time per step vs Threads
plt.figure(figsize=(6, 4))
plt.plot(df["Threads"], df["TimePerStep"], marker="o", linestyle='-')

plt.xlabel("Number of Threads")
plt.ylabel("Average Time per Step (s)")
plt.title(f"OpenMP Avg Time per Step for {body_count} Bodies")
plt.grid(True)
plt.tight_layout()

# Save and show
plt.savefig(f"OpenmpRuntime/openmp_time_per_step_{body_count}bodies.png", dpi=300)
# plt.show()