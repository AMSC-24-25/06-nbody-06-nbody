import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, PillowWriter
import re

def read_particle_positions(filename):
    """
    Reads particle_positions.txt into a list of Nx2 arrays (one per timestep).
    Returns: positions_list (length = frames, each is N x 2)
    """
    positions = []
    with open(filename, 'r') as f:
        curr_frame = []
        for line in f:
            line = line.strip()
            if not line:
                continue
            if line.startswith("# Timestep"):
                if curr_frame:
                    positions.append(np.array(curr_frame))
                    curr_frame = []
                continue
            parts = line.split()
            if len(parts) >= 3:
                curr_frame.append([float(parts[1]), float(parts[2])])
        if curr_frame:
            positions.append(np.array(curr_frame))
    return positions

# --- Read data ---
positions = read_particle_positions("../build/particle_positions.txt")
Nframes = len(positions)
N = positions[0].shape[0]

# --- Plot settings ---
fig, ax = plt.subplots(figsize=(6,6))
ax.set_xlim(-12, 12)
ax.set_ylim(-12, 12)
ax.set_aspect('equal')
scat = ax.scatter([], [], s=10, color="blue")

def animate(i):
    scat.set_offsets(positions[i])
    ax.set_title(f"Timestep {i*10}")  # If you saved every 10th step
    return scat,

ani = FuncAnimation(fig, animate, frames=Nframes, interval=50, blit=True)

# Save as GIF
ani.save("nbody_simulation.gif", writer=PillowWriter(fps=20))
print("Saved as nbody_simulation.gif")

