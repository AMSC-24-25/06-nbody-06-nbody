import matplotlib.pyplot as plt

def plot_points_from_file(file_path):
    # Initialize lists to hold coordinates over time
    x1_values, y1_values = [], []
    x2_values, y2_values = [], []

    # Read the file and process each line
    with open(file_path, 'r') as file:
        for line in file:
            line = line.strip()
            if line.startswith("(") and line.endswith(")"):  # Check if the line has coordinates
                coords = line[1:-1].split(",")  # Remove parentheses and split into X and Y
                x, y = float(coords[0]), float(coords[1])
                if len(x1_values) == len(x2_values):  # Alternate between point 1 and point 2
                    x1_values.append(x)
                    y1_values.append(y)
                else:
                    x2_values.append(x)
                    y2_values.append(y)

    # Plot the trajectories of the two points
    plt.figure(figsize=(8, 6))
    plt.plot(x1_values, y1_values, marker='o', label='Point 1 Trajectory')
    plt.plot(x2_values, y2_values, marker='s', label='Point 2 Trajectory')
    plt.xlabel('X Coordinate')
    plt.ylabel('Y Coordinate')
    plt.title('Trajectories of Two Points Over Time')
    plt.legend()
    plt.grid(True)
    plt.show()

# Example usage:
file_path = 'output.txt'  # Replace with your file name
plot_points_from_file(file_path)
