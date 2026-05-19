import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation


def load_csv(path):
    if not os.path.exists(path):
        raise FileNotFoundError(f"Nu găsesc fișierul: {path}")

    data = np.genfromtxt(path, delimiter=",", skip_header=1)

    if data.ndim == 1:
        data = data.reshape(1, -1)

    if data.shape[1] < 4:
        raise ValueError("CSV-ul trebuie să aibă cel puțin coloanele: time_s,x,y,z")

    t = data[:, 0]
    x = data[:, 1]
    y = data[:, 2]
    z = data[:, 3]

    if data.shape[1] >= 6:
        theta = data[:, 4]
        psi = data[:, 5]
    else:
        theta = np.degrees(np.arctan2(z, np.sqrt(x**2 + y**2)))
        psi = np.degrees(np.arctan2(y, x))

    return t, x, y, z, theta, psi


def main():
    t, x, y, z, theta, psi = load_csv("data.csv")

    fig = plt.figure(figsize=(8, 8))
    ax = fig.add_subplot(111, projection="3d")

    sphere_scale = 1.8

    # =========================
    # Sfera
    # =========================
    u = np.linspace(0, 2 * np.pi, 60)
    v = np.linspace(0, np.pi, 30)

    xs = sphere_scale * np.outer(np.cos(u), np.sin(v))
    ys = sphere_scale * np.outer(np.sin(u), np.sin(v))
    zs = sphere_scale * np.outer(np.ones_like(u), np.cos(v))

    ax.plot_wireframe(xs, ys, zs, linewidth=0.3, alpha=0.25)

    # =========================
    # Plan orizontal z = 0
    # =========================
    px = np.linspace(-sphere_scale, sphere_scale, 15)
    py = np.linspace(-sphere_scale, sphere_scale, 15)

    PX, PY = np.meshgrid(px, py)
    PZ = np.zeros_like(PX)

    ax.plot_surface(PX, PY, PZ, alpha=0.12)

    # =========================
    # Cercul intersecției dintre planul orizontal și sferă
    # =========================
    phi = np.linspace(0, 2 * np.pi, 400)

    equator_x = sphere_scale * np.cos(phi)
    equator_y = sphere_scale * np.sin(phi)
    equator_z = np.zeros_like(phi)

    ax.plot(
        equator_x,
        equator_y,
        equator_z,
        linewidth=2.5,
        alpha=0.9,
        label="Intersecție plan orizontal-sferă"
    )

    # =========================
    # Date scalate pentru desen
    # =========================
    x_plot = sphere_scale * x
    y_plot = sphere_scale * y
    z_plot = sphere_scale * z

    # =========================
    # Traiectorie completă
    # =========================
    ax.plot(
        x_plot,
        y_plot,
        z_plot,
        linewidth=1.5,
        alpha=0.35,
        label="Traiectorie calculată"
    )

    # =========================
    # Punct mobil
    # =========================
    moving_point, = ax.plot(
        [x_plot[0]],
        [y_plot[0]],
        [z_plot[0]],
        marker="o",
        markersize=8,
        linestyle="",
        label="Astru"
    )

    # Urma parcursă
    trace, = ax.plot(
        [x_plot[0]],
        [y_plot[0]],
        [z_plot[0]],
        linewidth=2,
        label="Urmărire"
    )

    # Vector O -> S
    radius_line, = ax.plot(
        [0, x_plot[0]],
        [0, y_plot[0]],
        [0, z_plot[0]],
        linestyle="--",
        linewidth=1.5
    )

    # Proiecția verticală pe planul z = 0
    vertical_projection, = ax.plot(
        [x_plot[0], x_plot[0]],
        [y_plot[0], y_plot[0]],
        [0, z_plot[0]],
        linestyle=":",
        linewidth=1.2
    )

    # Proiecția orizontală O -> P
    horizontal_projection, = ax.plot(
        [0, x_plot[0]],
        [0, y_plot[0]],
        [0, 0],
        linestyle="-.",
        linewidth=1.2
    )

    # Punct proiectat pe planul orizontal
    projected_point, = ax.plot(
        [x_plot[0]],
        [y_plot[0]],
        [0],
        marker="o",
        markersize=5,
        linestyle=""
    )

    # Text cu unghiurile
    info_text = ax.text2D(
        0.03,
        0.90,
        "",
        transform=ax.transAxes,
        fontsize=11,
        bbox=dict(boxstyle="round", alpha=0.15)
    )

    # =========================
    # Setări vizuale
    # =========================
    lim = sphere_scale * 1.15

    ax.set_xlim(-lim, lim)
    ax.set_ylim(-lim, lim)
    ax.set_zlim(-lim, lim)

    ax.set_box_aspect((1, 1, 1))
    ax.set_axis_off()
    ax.legend(loc="upper right")

    # =========================
    # Update animație
    # =========================
    def update(frame):
        moving_point.set_data(
            [x_plot[frame]],
            [y_plot[frame]]
        )
        moving_point.set_3d_properties(
            [z_plot[frame]]
        )

        trace.set_data(
            x_plot[:frame + 1],
            y_plot[:frame + 1]
        )
        trace.set_3d_properties(
            z_plot[:frame + 1]
        )

        radius_line.set_data(
            [0, x_plot[frame]],
            [0, y_plot[frame]]
        )
        radius_line.set_3d_properties(
            [0, z_plot[frame]]
        )

        vertical_projection.set_data(
            [x_plot[frame], x_plot[frame]],
            [y_plot[frame], y_plot[frame]]
        )
        vertical_projection.set_3d_properties(
            [0, z_plot[frame]]
        )

        horizontal_projection.set_data(
            [0, x_plot[frame]],
            [0, y_plot[frame]]
        )
        horizontal_projection.set_3d_properties(
            [0, 0]
        )

        projected_point.set_data(
            [x_plot[frame]],
            [y_plot[frame]]
        )
        projected_point.set_3d_properties(
            [0]
        )

        hours = t[frame] / 3600.0

        info_text.set_text(
            f"t = {hours:.2f} h\n"
            f"$\\theta$ = {theta[frame]:.2f}°\n"
            f"$\\psi$ = {psi[frame]:.2f}°"
        )

        ax.set_title("Simulare urmărire astru")

        return (
            moving_point,
            trace,
            radius_line,
            vertical_projection,
            horizontal_projection,
            projected_point,
            info_text
        )

    # =========================
    # Viteză animație
    # =========================
    speed_factor = 8
    frame_indices = range(0, len(t), speed_factor)

    ani = FuncAnimation(
        fig,
        update,
        frames=frame_indices,
        interval=10,
        blit=False,
        repeat=True,
        cache_frame_data=False
    )

    plt.show()


if __name__ == "__main__":
    main()