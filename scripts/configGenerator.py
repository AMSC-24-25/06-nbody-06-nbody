import random
import math

nbodies = 50

# Function to generate a point within 10 to 20 units from the origin
def generate_point_within_range(min_dist, max_dist):
    distance = random.uniform(min_dist, max_dist)
    angle = random.uniform(0, 2 * math.pi)
    x = distance * math.cos(angle)
    y = distance * math.sin(angle)
    return x, y

# Function to compute tangent velocity (perpendicular vector to the radius)
def compute_tangent_velocity(x, y):
    magnitude = random.uniform(1, 5)  # Random velocity magnitude between 1 and 5
    # Tangent vector is perpendicular: (-y, x)
    tangent_x = -y
    tangent_y = x
    # Normalize and scale by magnitude
    length = math.sqrt(tangent_x**2 + tangent_y**2)
    tangent_x = (tangent_x / length) * magnitude
    tangent_y = (tangent_y / length) * magnitude
    return tangent_x, tangent_y

# Generate 100 points and their tangent velocities
points_with_velocities = []
for _ in range(nbodies):
    x, y = generate_point_within_range(10, 20)
    vx, vy = compute_tangent_velocity(x, y)
    points_with_velocities.append((x, y, vx, vy))

# Write points and their tangent velocities to a text file
with open("input.txt", "w") as file:
    file.write(f"{nbodies}\n")
    for x, y, vx, vy in points_with_velocities:
        file.write(f"1.0\n{x} {y}\n{vx} {vy}\n")