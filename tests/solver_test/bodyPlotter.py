import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

def animate_particles(file_path, fig_size=(5, 5)):
    # Initialize containers for data
    particle_positions = {}
    particle_masses = []

    with open(file_path, 'r') as file:
        lines = file.readlines()
        
        # Read number of particles
        num_particles = int(lines[0].strip())
        
        # Read masses of particles
        for i in range(1, num_particles + 1):
            particle_masses.append(float(lines[i].strip()))
        
        # Read positions for each particle
        position_lines = lines[num_particles + 1:]
        for i in range(num_particles):
            particle_positions[i] = ([], [])  # x and y

        for frame in range(0, len(position_lines), num_particles):
            for i in range(num_particles):
                x, y = map(float, position_lines[frame + i].strip().split())
                particle_positions[i][0].append(x)
                particle_positions[i][1].append(y)

    # Set up the figure and axis
    fig, ax = plt.subplots(figsize=fig_size)
    all_x = [x for pos in particle_positions.values() for x in pos[0]]
    all_y = [y for pos in particle_positions.values() for y in pos[1]]
    ax.set_xlim(min(all_x) - 1, max(all_x) + 1)
    ax.set_ylim(min(all_y) - 1, max(all_y) + 1)
    ax.set_aspect('equal')  # Ensure equal scaling of x and y axes
    ax.set_xlabel('X Coordinate')
    ax.set_ylabel('Y Coordinate')
    ax.set_title('Animated Trajectories of Particles')

    # Initialize markers and trajectories for all particles
    particle_markers = []
    particle_trajectories = []

    for i in range(num_particles):
        marker, = ax.plot([], [], marker='o', label=f'Particle {i + 1} (Mass: {particle_masses[i]:.2f})')
        trajectory, = ax.plot([], [], linestyle='--', alpha=0.5)
        particle_markers.append(marker)
        particle_trajectories.append(trajectory)

    ax.legend()

    # Update function for animation
    def update(frame):
        for i in range(num_particles):
            # Update position and trajectory
            x_vals, y_vals = particle_positions[i]
            particle_markers[i].set_data([x_vals[frame]], [y_vals[frame]])
            particle_trajectories[i].set_data(x_vals[:frame + 1], y_vals[:frame + 1])
        return particle_markers + particle_trajectories

    # Create the animation
    frames = min(len(particle_positions[i][0]) for i in range(num_particles))
    ani = FuncAnimation(fig, update, frames=frames, interval=1, blit=False)

    # Show the animation
    plt.show()
    
# Example usage:
file_path = 'build/bodyData.txt'  # Replace with your file name
animate_particles(file_path, fig_size=(5, 5))
