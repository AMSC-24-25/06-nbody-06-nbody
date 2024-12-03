import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

def animate_particles_with_vectors(file_path, num_particles):
    # Initialize lists to hold coordinates and vectors for all particles
    particle_positions = {i: ([], []) for i in range(num_particles)}  # {0: ([x1], [y1]), 1: ([x2], [y2]), ...}
    particle_vectors = {i: ([], []) for i in range(num_particles)}   # {0: ([vx1], [vy1]), 1: ([vx2], [vy2]), ...}

    # Read the file and process lines
    with open(file_path, 'r') as file:
        lines = [line.strip() for line in file if line.strip()]
        for i in range(0, len(lines), 2):  # Process every two lines
            x, y = map(float, lines[i][1:-1].split(","))
            vx, vy = map(float, lines[i + 1][1:-1].split(","))
            particle_idx = (i // 2) % num_particles
            particle_positions[particle_idx][0].append(x)
            particle_positions[particle_idx][1].append(y)
            particle_vectors[particle_idx][0].append(vx)
            particle_vectors[particle_idx][1].append(vy)

    # Set up the figure and axis
    fig, ax = plt.subplots()
    all_x = [x for pos in particle_positions.values() for x in pos[0]]
    all_y = [y for pos in particle_positions.values() for y in pos[1]]
    ax.set_xlim(min(all_x) - 1, max(all_x) + 1)
    ax.set_ylim(min(all_y) - 1, max(all_y) + 1)
    ax.set_aspect('equal')
    ax.set_xlabel('X Coordinate')
    ax.set_ylabel('Y Coordinate')
    ax.set_title('Animated Trajectories and Vectors of Particles')

    # Initialize markers, trajectories, and vectors for all particles
    particle_markers = []
    particle_trajectories = []
    particle_vectors_quivers = []

    for i in range(num_particles):
        marker, = ax.plot([], [], marker='o', label=f'Particle {i + 1}')
        trajectory, = ax.plot([], [], linestyle='--', alpha=0.5)
        quiver = ax.quiver([], [], [], [], angles='xy', scale_units='xy', scale=1, color='k')
        particle_markers.append(marker)
        particle_trajectories.append(trajectory)
        particle_vectors_quivers.append(quiver)

    ax.legend()

    # Update function for animation
    def update(frame):
        for i in range(num_particles):
            # Update position and trajectory
            x_vals, y_vals = particle_positions[i]
            vx_vals, vy_vals = particle_vectors[i]
            particle_markers[i].set_data([x_vals[frame]], [y_vals[frame]])
            particle_trajectories[i].set_data(x_vals[:frame + 1], y_vals[:frame + 1])
            
            # Update vector (quiver)
            particle_vectors_quivers[i].set_offsets([x_vals[frame], y_vals[frame]])
            particle_vectors_quivers[i].set_UVC(vx_vals[frame], vy_vals[frame])
        return particle_markers + particle_trajectories + particle_vectors_quivers

    # Create the animation
    frames = min(len(particle_positions[i][0]) for i in range(num_particles))
    ani = FuncAnimation(fig, update, frames=frames, interval=0.5, blit=False)

    plt.show()

# Example usage:
file_path = 'output.txt'  # Replace with your file name
num_particles = 3  # Adjust this for the number of particles
animate_particles_with_vectors(file_path, num_particles)
