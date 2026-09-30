# MTRN4231

Course code for MTRN4231 (UNSW Mechatronics): demo packages, utilities, lab workspaces, and a Docker environment that runs all of it (ROS 2 Humble + UR5e driver + MoveIt) on a Mac or a Linux PC.

## Start here

```bash
git clone https://github.com/jadegav10/MTRN4231.git ~/4231
```

Then choose how to run ROS 2:

| Your computer | What to do |
| --- | --- |
| **Mac, or Linux without ROS** | Use the Docker environment: follow the **[Setup Guide](setup/README.md)** (about 30 minutes, once) |
| **Ubuntu 22.04 with ROS 2 Humble** (lab PCs, dual boot) | Follow the course Install Guide on Moodle, then `sudo apt install ros-humble-ur` |

To get course updates later: `cd ~/4231 && git pull`.

## What's in the repository

```
MTRN4231/
├── setup/        Setup Guide + the Docker environment
├── labs/         lab1_workspace … lab5_workspace (lab sheets are inside each one)
├── packages/     4231_demo_packages and 4231_utils: examples and helpers
├── scripts/      setupRealur5e.sh, setupFakeur5e.sh, camera.sh (native Ubuntu)
└── source/       course-modified MoveIt 2 and UR driver source (optional, see below)
```

> **Paths in the lab sheets:** the PDFs were written before this layout, so add `labs/` or `packages/` to the paths they give.
> For example, `~/4231/lab2_workspace` is now `~/4231/labs/lab2_workspace`, and `~/4231/4231_demo_packages` is now `~/4231/packages/4231_demo_packages`.

Keep your own work in `labs/` or in your own packages, so `git pull` never conflicts.
For a backup, push to a **private** repo of your own.
Don't use a public fork: it would publish your lab solutions.

---

## Course overview

Welcome to 4231, hope you enjoy this course and learning ROS.
Demo, utility and stub files are provided to help you learn.

### `packages/4231_demo_packages`

* **demo_client_service**: client example, service example
* **demo_interactive_marker**: basic interactive commands
* **demo_launch_param**: RViz marker example, launch file example
* **demo_moveit_ur**: basic movement, collision avoidance, constrained path, and servo movement examples
* **demo_tf2**: static frame, dynamic frame, and listener examples

### `packages/4231_utils`

* **util_arduino_serial**: opens serial communication with an Arduino, and passes through string messages from a topic
* **util_keyboard**: publishes key presses to a topic

Build both workspaces once: `./4231.sh build` in Docker, or `colcon build` in each folder on Ubuntu.

### `scripts`

* **camera.sh**: launches the ROS 2 camera interface
* **setupFakeur5e.sh**: launches the UR driver, the MoveIt server, and RViz with the motion planner, in visualisation mode
* **setupRealur5e.sh**: the same, connected to the physical robot at `192.168.0.100`

These open `gnome-terminal` windows, so they're for native Ubuntu.
In Docker, use the desktop icons or `./4231.sh fake` / `./4231.sh real` instead ([Setup Guide](setup/README.md#connecting-to-the-ur5e)).

### `labs`

Workspaces for lab and project work. Lab instructions (PDF) are inside each directory.

### `source/ros_ur_driver` and `source/ws_moveit2`

Source copies of the Universal Robots ROS 2 driver and MoveIt 2, with some files changed for course convenience.
The Docker environment doesn't use them: it installs both from apt.
On Ubuntu, only build them if you need them to match the lab machines exactly. A full MoveIt rebuild takes 30+ minutes.

*Edit at your own risk.* If rebuilding, follow the instructions on GitHub carefully.
