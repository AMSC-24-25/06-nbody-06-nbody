import matplotlib.pyplot as plt

def plot_data(file_path):
    # Open the file and read lines
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Extract the reference value from the first line
    first_line = lines[0].strip().split()
    reference_value = float(first_line[1])

    # Extract the x and y coordinates from the remaining lines
    x_coords = []
    y_coords = []
    for line in lines[1:]:
        x, y = map(float, line.strip().split())
        x_coords.append(x)
        y_coords.append(y)

    # Create the plot
    plt.figure(figsize=(8, 6))
    
    # Plot the blue line connecting the coordinates
    plt.plot(x_coords, y_coords, color='blue', label='Energy')
    
    # Plot the red horizontal reference line
    plt.axhline(y=reference_value, color='red', linestyle='--', label=f'Initial Energy (E={reference_value})')

    # Add labels and title
    plt.xlabel('Timestep')
    plt.ylabel('Total Energy (K+U)')
    plt.title('Energy of the system over time')
    plt.legend()

    # Show the plot
    plt.grid(True)
    plt.show()

plot_data('energyData.txt')