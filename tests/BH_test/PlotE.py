import numpy as np
import matplotlib.pyplot as plt
import re

def read_energy_data(filename):
    """Read particle energy data from file and return timesteps and total energies."""
    timesteps = []
    total_energies = []
    current_energies = []
    
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('# Format'):
                continue
                
            # Check for timestep header
            timestep_match = re.search(r'# Timestep (\d+)', line)
            if timestep_match:
                if current_energies:
                    total_energies.append(sum(current_energies))
                    current_energies = []
                timesteps.append(int(timestep_match.group(1)))
                continue
            
            # Parse particle data if it's not a comment line
            if not line.startswith('#'):
                try:
                    parts = line.split()
                    if len(parts) == 4:  # ParticleID, X, Y, E
                        energy = float(parts[3])
                        current_energies.append(energy)
                except ValueError:
                    continue
    
    # Don't forget to add the last timestep
    if current_energies:
        total_energies.append(sum(current_energies))
    
    return np.array(timesteps), np.array(total_energies)

def save_energy_plot(filename, annotation_stride=1000, y_margin=0.01):
    """Create and save an energy plot with zoomed y-axis and selective annotations."""
    # Read the data
    timesteps, total_energies = read_energy_data(filename)
    
    # Create the plot with a white background
    plt.figure(figsize=(12, 8))
    plt.style.use('default')
    
    # Plot with line
    plt.plot(timesteps, total_energies, '-', 
             linewidth=1.5, 
             color='blue',
             label='Total Energy',
             alpha=0.8)
    
    # Add annotations at strided intervals
    for i in range(0, len(timesteps), annotation_stride):
        plt.annotate(f'{total_energies[i]:.6f}', 
                    (timesteps[i], total_energies[i]),
                    xytext=(0, 10), 
                    textcoords='offset points',
                    ha='center',
                    va='bottom',
                    fontsize=8,
                    rotation=45)
    
    # Calculate y-axis limits with margin
    y_min = np.min(total_energies)
    y_max = np.max(total_energies)
    y_range = y_max - y_min
    plt.ylim([y_min - y_range * y_margin, y_max + y_range * y_margin])
    
    # Customize the plot
    plt.xlabel('Timestep', fontsize=12)
    plt.ylabel('Total Energy', fontsize=12)
    plt.title('Total System Energy', fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.3)
    
    # Format x-axis to use scientific notation
    plt.ticklabel_format(style='sci', axis='x', scilimits=(0,0))
    
    plt.legend()
    plt.margins(x=0.02)  # Reduce margins
    plt.tight_layout()
    
    # Save the plot
    plt.savefig('energy_plot.png', dpi=300, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":
    # Adjust annotation_stride to control how often values are shown
    # Adjust y_margin to control zoom level (smaller value = more zoom)
    save_energy_plot("particle_positions.txt", annotation_stride=100, y_margin=0.1)