import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

def plot_deltae_vs_time(file_path):
    # Initialize containers for time and DeltaE
    times = []
    delta_es = []

    with open(file_path, 'r') as file:
        lines = file.readlines()
        
        # Parse the file
        for i in range(0, len(lines), 3):  # Each block has 3 lines
            time_line = lines[i].strip()
            deltae_line = lines[i + 2].strip()

            # Extract Time and DeltaE
            time = float(time_line.split(':')[1].strip())
            delta_e = float(deltae_line.split(':')[1].strip())

            times.append(time)
            delta_es.append(delta_e)

    # Plot DeltaE vs Time
    plt.figure(figsize=(8, 5))
    plt.plot(times, delta_es, marker='o', linestyle='-', color='b', label='ΔE')
    plt.xlabel('Time (T)')
    plt.ylabel('ΔE (Delta Energy)')
    plt.title('Delta Energy vs Time')
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    # Show the plot
    plt.show()

def plot_deltae_vs_time_loglog(file_path):
    # Initialize containers for time and DeltaE
    times = []
    delta_es = []

    with open(file_path, 'r') as file:
        lines = file.readlines()
        
        # Parse the file
        for i in range(0, len(lines), 3):  # Each block has 3 lines
            time_line = lines[i].strip()
            deltae_line = lines[i + 2].strip()

            # Extract Time and DeltaE
            time = float(time_line.split(':')[1].strip())
            delta_e = float(deltae_line.split(':')[1].strip())

            times.append(time)
            delta_es.append(delta_e)

    # Plot DeltaE vs Time on log-log scale
    plt.figure(figsize=(8, 5))
    plt.loglog(times, delta_es, marker='o', linestyle='-', color='b', label='ΔE')
    plt.xlabel('Time (T)')
    plt.ylabel('ΔE (Delta Energy)')
    plt.title('Delta Energy vs Time (Log-Log Scale)')
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()

    # Show the plot
    plt.show()

# Example usage:
file_path = 'build/energyData.txt'  # Replace with your file name
plot_deltae_vs_time(file_path)
plot_deltae_vs_time_loglog(file_path)
