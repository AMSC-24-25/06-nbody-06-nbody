import numpy as np
import matplotlib.pyplot as plt

def read_and_plot_energies(filename):
    timesteps = []
    tot_energies = []
    
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    for line in lines:
        if line.startswith('# Timestep'):
            current_timestep = int(line.split()[2])
        elif not line.startswith('#') and line.strip():
            try:
                tot = float(line.strip())
                timesteps.append(current_timestep)
                tot_energies.append(tot)
            except (ValueError, IndexError):
                continue

    plt.figure(figsize=(12, 6))
    plt.plot(timesteps, tot_energies, '-b', label='Total Energy', linewidth=2)
    plt.xlabel('Timestep')
    plt.ylabel('Total Energy')
    plt.title('System Total Energy vs. Timestep')
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.savefig('energy_evolution.png', dpi=300, bbox_inches='tight')
    plt.close()

read_and_plot_energies('energy_evolution.txt')
