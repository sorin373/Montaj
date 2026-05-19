import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation


def load_csv(path):
    # if not os.path.exists(path):
    #     raise FileNotFoundError(f"Nu găsesc fișierul: {path}")

    data = np.genfromtxt(path, delimiter=",", skip_header=1)

    if data.ndim == 1:
        data = data.reshape(1, -1)

    if data.shape[1] < 4:
        raise ValueError("CSV-ul trebuie să aibă cel puțin coloanele: time_s,x,y,z")

    t = data[:, 0]
    x = data[:, 1]
    y = data[:, 2]
    z = data[:, 3]

    return t, x, y, z


def main():
    t, x, y, z = load_csv("data.csv")

    fig = plt.figure(figsize=(8, 8))
    ax = fig.add_subplot(111, projection="3d")

    u = np.linspace(0, 2 * np.pi, 80)
    v = np.linspace(0, np.pi, 40)

    sphere_scale = 1.8

    xs = sphere_scale * np.outer(np.cos(u), np.sin(v))
    ys = sphere_scale * np.outer(np.sin(u), np.sin(v))
    zs = sphere_scale * np.outer(np.ones_like(u), np.cos(v))

    x_plot = sphere_scale * x
    y_plot = sphere_scale * y
    z_plot = sphere_scale * z

    ax.plot_wireframe(xs, ys, zs, linewidth=0.3, alpha=0.25)

    ax.plot(x_plot, y_plot, z_plot, linewidth=1.5, alpha=0.35, label="Traiectorie completă")

    moving_point, = ax.plot(
        [x_plot[0]], [y_plot[0]], [z_plot[0]],
        marker="o",
        markersize=8,
        linestyle=""
    )

    trace, = ax.plot(
        [x_plot[0]], [y_plot[0]], [z_plot[0]],
        linewidth=2,
        label="Urmărire"
    )

    ax.set_xlim(-1.5, 1.5)
    ax.set_ylim(-1.5, 1.5)
    ax.set_zlim(-1.5, 1.5)

    ax.set_box_aspect((1, 1, 1))

    ax.set_axis_off()

    ax.legend()

    def update(frame):
        moving_point.set_data([x_plot[frame]], [y_plot[frame]])
        moving_point.set_3d_properties([z_plot[frame]])

        trace.set_data(x_plot[:frame + 1], y_plot[:frame + 1])
        trace.set_3d_properties(z_plot[:frame + 1])

        hours = t[frame] / 3600.0
        ax.set_title(f"Simulare urmărire astru - t = {hours:.2f} h")

        return moving_point, trace

    ani = FuncAnimation(
        fig,
        update,
        frames=len(t),
        interval=1,
        blit=False,
        repeat=True
    )

    plt.show()


if __name__ == "__main__":
    main()