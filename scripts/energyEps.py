import matplotlib.pyplot as plt

# List of data files and their corresponding colors
data_files = {
    "energyData-15.txt": "red",
    "energyData-10.txt": "blue",
    "energyData-5.txt": "green",
    "energyData0.txt": "purple"
}

label = {"energyData-15.txt": "eps = 1e-15",
         "energyData-10.txt": "eps = 1e-10",
         "energyData-5.txt": "eps = 1e-5",
         "energyData0.txt": "eps = 0"
         }

plt.figure(figsize=(8, 6))

for filename, color in data_files.items():
    x_vals = []
    y_vals = []
    try:
        with open(filename, "r") as f:
            for line in f:
                if line.strip():  # Skip empty lines
                    parts = line.strip().split()
                    if len(parts) == 2:
                        x, y = map(float, parts)
                        x_vals.append(x)
                        y_vals.append(y)
        plt.plot(x_vals, y_vals, color=color,
                 label=label[filename])  # Line plot
    except FileNotFoundError:
        print(f"File not found: {filename}")

plt.xlabel("Timestep")
plt.ylabel("Total Energy (K+U)")
plt.title("Energy Data Plot")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
