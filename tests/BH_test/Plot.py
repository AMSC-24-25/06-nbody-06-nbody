import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, PillowWriter
import re


def read_particle_data(filename):
    timesteps = {}
    current_timestep = None

    with open(filename, "r") as f:
        for line in f:
            if not line.strip():
                continue
            if line.startswith("#"):
                timestep_match = re.search(r"Timestep (\d+)", line)
                if timestep_match:
                    current_timestep = int(timestep_match.group(1))
                    timesteps[current_timestep] = []
                continue
            try:
                values = line.strip().split()
                particle_id, x, y = map(float, values[:3])
                timesteps[current_timestep].append([x, y])
            except ValueError:
                continue

    for timestep in timesteps:
        timesteps[timestep] = np.array(timesteps[timestep])
    return timesteps


def setup_plot():
    colors = [
        "#1f77b4",
        "#d62728",
        "#2ca02c",
        "#9467bd",
        "#8c564b",
        "#e377c2",
        "#7f7f7f",
        "#17becf",
        "#ff7f0e",
        "#bcbd22",
    ]
    plt.style.use("dark_background")
    fig, ax = plt.subplots(figsize=(12, 4), facecolor="black")
    ax.set_facecolor("black")
    ax.yaxis.set_major_formatter(plt.FormatStrFormatter("%.2f"))
    ax.xaxis.set_major_formatter(plt.FormatStrFormatter("%.1f"))
    ax.tick_params(axis="both", which="major", labelsize=10, direction="out", length=6)
    return fig, ax, colors


def plot_frame(ax, positions, particle_trajectories, colors, frame, limits):
    ax.clear()
    ax.set_facecolor("black")
    ax.set_xlim(limits[0], limits[1])
    ax.set_ylim(limits[2], limits[3])

    # Set aspect ratio to match reference image
    ax.set_aspect((limits[1] - limits[0]) / (limits[3] - limits[2]) * 0.333)

    # Set major tick intervals based on data range
    x_range = limits[1] - limits[0]
    y_range = limits[3] - limits[2]
    ax.xaxis.set_major_locator(plt.MultipleLocator(x_range / 6))
    ax.yaxis.set_major_locator(plt.MultipleLocator(y_range / 8))

    ax.grid(True, linestyle="--", alpha=0.2, color="gray")
    ax.set_xlabel("x", color="white")
    ax.set_ylabel("Y Position", color="white")

    for i, pos in enumerate(positions):
        particle_trajectories[i].append(pos)
        if len(particle_trajectories[i]) > 1:
            trajectory = np.array(particle_trajectories[i])
            ax.plot(
                trajectory[:, 0],
                trajectory[:, 1],
                "-",
                color=colors[i % len(colors)],
                alpha=0.6,
                linewidth=1.5,
            )
        ax.scatter(
            pos[0],
            pos[1],
            color=colors[i % len(colors)],
            s=100,
            alpha=1.0,
            label=f"Particle {i}",
            edgecolor="white",
            linewidth=1,
        )

    ax.set_title(f"Particle Positions - Timestep {frame}", color="white", pad=20)
    legend = ax.legend(bbox_to_anchor=(1.05, 1), loc="upper left")
    plt.setp(legend.get_texts(), color="white")
    ax.tick_params(colors="white")
    for spine in ax.spines.values():
        spine.set_color("white")


def save_visualization(filename, fps=20, interval=800):
    timesteps = read_particle_data(filename)
    if not timesteps:
        return

    # Calculate plot limits with appropriate ratio
    all_positions = np.concatenate([pos for pos in timesteps.values()])
    margin = 0.1
    x_min, y_min = all_positions.min(axis=0)
    x_max, y_max = all_positions.max(axis=0)
    x_range = x_max - x_min
    y_range = y_max - y_min

    # Adjust x limits to maintain desired aspect ratio
    target_ratio = 3.0  # Width should be 3x the height
    current_ratio = x_range / y_range
    if current_ratio < target_ratio:
        extra_x = (target_ratio * y_range - x_range) / 2
        x_min -= extra_x
        x_max += extra_x

    limits = [
        x_min - margin * x_range,
        x_max + margin * x_range,
        y_min - margin * y_range,
        y_max + margin * y_range,
    ]

    # Create animation
    fig, ax, colors = setup_plot()
    particle_trajectories = {i: [] for i in range(len(timesteps[0]))}

    def update(frame):
        if frame in timesteps:
            plot_frame(
                ax, timesteps[frame], particle_trajectories, colors, frame, limits
            )
        return (ax,)

    timestep_keys = sorted(timesteps.keys())
    anim = FuncAnimation(
        fig, update, frames=timestep_keys, interval=interval, blit=False
    )

    plt.tight_layout()
    writer = PillowWriter(fps=fps)
    anim.save("particle_animation.gif", writer=writer, dpi=150)
    plt.close()

    # Save final state
    fig, ax, colors = setup_plot()
    plot_frame(
        ax,
        timesteps[max(timesteps.keys())],
        {
            i: [timesteps[t][i] for t in sorted(timesteps.keys())]
            for i in range(len(timesteps[0]))
        },
        colors,
        max(timesteps.keys()),
        limits,
    )
    plt.tight_layout()
    plt.savefig("final_state.png", dpi=150, bbox_inches="tight", facecolor="black")
    plt.close()


if __name__ == "__main__":
    save_visualization("particle_positions.txt")
