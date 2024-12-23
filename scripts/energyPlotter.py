import matplotlib.pyplot as plt

def plot_multiple_data(file_paths):
    colors = ['blue', 'green', 'orange', 'purple', 'cyan']  # Add more colors if needed

    plt.figure(figsize=(10, 8))

    for i, file_path in enumerate(file_paths):
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

        # Plot the data line with a unique color
        color = colors[i % len(colors)]
        plt.plot(x_coords, y_coords, color=color, label=f'Data {i}')

        # Plot the red horizontal reference line only for the first dataset
        if i == 0:
            plt.axhline(y=reference_value, color='red', linestyle='--', label=f'Reference (Y={reference_value})')

    # Add labels and title
    plt.xlabel('Timestep')
    plt.ylabel('Total Energy (K+U)')
    plt.title('Energy of the system over time')
    plt.legend()

    # Show the plot
    plt.grid(True)
    plt.show()

# Replace ['file1.txt', 'file2.txt'] with the paths to your files
plot_multiple_data(['energyData.txt'])
