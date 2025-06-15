import numpy as np
import matplotlib.pyplot as plt

# def read_and_plot_energies(filename):
def read_and_plot_energies(filename, output_image):
    timesteps = []
    total_energies = []
    
    with open(filename, 'r') as f:
        lines = f.readlines()
        
    current_sum = 0
    energy_count = 0
    
    for line in lines:
        if line.startswith('# Timestep'):
            if energy_count > 0:
                timesteps.append(current_timestep)
                total_energies.append(current_sum)
            current_timestep = int(line.split()[2])
            current_sum = 0
            energy_count = 0
        elif not line.startswith('#') and line.strip():
            try:
                energy = float(line.strip().split()[3])
                current_sum += energy
                energy_count += 1
            except (ValueError, IndexError):
                continue
    
    # Add the last timestep
    if energy_count > 0:
        timesteps.append(current_timestep)
        total_energies.append(current_sum)
    
    # Create plot
    plt.figure(figsize=(12, 6))
    
    # Plot with connected lines
    plt.plot(timesteps, total_energies, '-b', linewidth=1)
    
    # Set fewer y-axis ticks to avoid overlapping
    min_energy = min(total_energies)
    max_energy = max(total_energies)
    # Reduce number of ticks by increasing the interval
    num_ticks = 8  # Adjust this number to control density of y-axis labels
    yticks = np.linspace(min_energy, max_energy, num_ticks)
    plt.yticks(yticks)
    
    plt.xlabel('Timestep')
    plt.ylabel('Total Energy')
    plt.title('Total System Energy vs. Timestep')
    plt.grid(True, alpha=0.3)
    
    # Format axes
    plt.ticklabel_format(style='sci', axis='x', scilimits=(0,0))  # Scientific notation for x-axis only
    plt.ticklabel_format(useOffset=False, axis='y')  # No offset for y-axis
    
    # Print some values to verify
    print(f"First few values:")
    for i in range(min(5, len(timesteps))):
        print(f"Timestep {timesteps[i]}: {total_energies[i]}")
    
    # plt.savefig('energy_evolution.png', dpi=300, bbox_inches='tight')
    plt.savefig(output_image, dpi=300, bbox_inches='tight')
    plt.close()

# read_and_plot_energies('particle_positions.txt')
import sys
import os

if __name__ == "__main__":
    filename = sys.argv[1] if len(sys.argv) > 1 else "particle_positions.txt"
    
    if "openmp" in filename.lower():
        output_image = "energy_evolution_openmp.png"
    else:
        output_image = "energy_evolution.png"

    # read_and_plot_energies(filename)
    read_and_plot_energies(filename, output_image)