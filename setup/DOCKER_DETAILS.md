# MTRN4231 Docker environment: technical details

Students should start with the setup guide in [`README.md`](README.md). This page is for maintainers.

This folder builds a ROS 2 Humble (Ubuntu 22.04) container for all of the supplied code.
The repository root (the folder above `setup/`) is **bind-mounted** at `~/4231` inside the container.
Anything you edit or build there is the same folder on your computer, so it survives container rebuilds.
Lab workspaces are at `~/4231/labs/lab1_workspace/...`.

The image follows the **MTRN4231 Install Guide**: Ubuntu 22.04 + ROS 2 Humble **desktop**, the guide's useful installs, and the UR driver from apt (`ros-humble-ur`, which brings in MoveIt 2 as apt binaries).
**MoveIt is not built from source**, so `source/ws_moveit2` and `source/ros_ur_driver` aren't used.
The image contains no course code:

| Needed for | What is installed (all apt, except pip where noted) |
|---|---|
| Install Guide | `ros-humble-desktop`, colcon, pip, `opencv-python` (pip), `ros-humble-tf-transformations` |
| UR5e + MoveIt | `ros-humble-ur`, `ros-humble-moveit`, `moveit_visual_tools` |
| Lab 1–2 | rqt (all plugins), rviz2, tf2_tools, `example_interfaces` |
| Lab 3 depth camera | `realsense2_camera`, `librealsense2`, `cv_bridge`, `pyrealsense2` (pip) |
| Lab 4 YOLO | `v4l2_camera`, `image_view`, rosbag2 (ultralytics is pip-installed per the lab sheet) |
| Lab 5 URDF | `xacro`, `joint_state_publisher_gui`, `ur_description` |
| 4231_utils | `pyserial` (Arduino), `pygame` (keyboard) |

The guide's Arduino IDE and VS Code belong on the host, not in the container.

```
setup/
├── README.md        ← the student setup guide (+ images/)
├── 4231.sh          ← helper: up / shell / build / fake / real / desktop / stop
├── compose.yaml     ← services: `linux` (lab PCs), `mac-host` (Docker Desktop, default), `mac` (fallback)
├── Dockerfile
├── .env.example     ← copied to .env on first run
├── entrypoint.sh    ← starts the noVNC desktop on macOS
├── ros_env.sh       ← sourced in every shell (ROS + course workspaces)
└── scripts/         ← on PATH in the container
    ├── build_4231.sh    builds packages/4231_utils and packages/4231_demo_packages
    ├── ur5e_fake.sh     container version of scripts/setupFakeur5e.sh
    └── ur5e_real.sh     container version of scripts/setupRealur5e.sh
```

---

## Quick start

```bash
cd setup
./4231.sh up        # first run builds the image (~15–25 min) and creates .env
./4231.sh build     # colcon-builds packages/4231_utils + packages/4231_demo_packages (about a minute)
./4231.sh fake      # fake UR5e + MoveIt + RViz: the "does it all work" test
./4231.sh shell     # open more terminals, as many as you like
```

`./4231.sh` picks the `linux` or `mac` service automatically.
You can use plain compose instead: `docker compose up -d linux`, then `docker exec -it mtrn4231 bash`.

The build output (`build/ install/ log/`) is written into each workspace in the repository (git ignores it).

Every new shell sources ROS 2 (including MoveIt and the UR driver) and those two workspaces automatically.
Students still `source install/setup.bash` in their own lab workspace, as the lab sheets say.

### The desktop

* **macOS:** a full XFCE Linux desktop runs in the container, with a panel, terminals, a file manager and a text editor.
  * **Browser:** <http://localhost:6080/vnc.html?autoconnect=1&resize=remote>, or run `./4231.sh desktop`.
  * **Native client:** *Finder → Go → Connect to Server → `vnc://localhost:5901`*. This needs `VNC_PASSWORD` set in `.env`.
  * Desktop icons: **4231 Terminal** (opens in `~/4231` with ROS sourced), **Connect UR5e (real)**, **UR5e (fake hardware)** and **4231 Code**.
  * Both ports listen on localhost only, so nobody else on the network can see the desktop.
* **Linux:** GUIs open straight on your own desktop through the host X server. `4231.sh` runs `xhost +local:`.

---

## Connecting to a UR5e

The UR ROS 2 driver works in two directions:

1. The PC connects **to** the robot (ports 29999, 30001–30004).
2. The robot's *External Control* program connects **back to** the PC on ports **50001–50004**.
   It uses the "Host IP" typed into the URCap and the `reverse_ip` that the driver embeds in the script it sends.

Both directions must work through Docker.

### On the pendant (both platforms)

1. Put the robot in **Remote/Manual** mode as the lab requires, then power on and release the brakes.
2. Create a program that contains only the **External Control** URCap node.
3. In *Installation → URCaps → External Control*, set **Host IP** to *this computer's* IP on the robot network, and keep port 50002.
4. Start the driver (below), then press **Play** on the pendant.
   The driver terminal should print `Robot connected to reverse interface. Ready to receive control commands.`

### Linux lab PC (recommended)

The `linux` service uses **host networking**, so the container shares the PC's network stack and nothing needs forwarding.

1. Plug in the orange Ethernet cable. Give that interface a static IP on the robot subnet, e.g. `192.168.0.10/24`.
2. Check the connection with `ping 192.168.0.100`.
3. Run `./4231.sh real`, or `./4231.sh real 192.168.0.xxx` for a different robot.

Leave `HOST_IP` empty in `.env`; the driver detects it automatically.
**Give each bench a different `ROS_DOMAIN_ID`** in `.env`. Host networking means ROS traffic reaches the whole lab network, and benches sharing a domain would see (and command) each other's robots.

### macOS (Docker Desktop): tested with a UR5e on PolyScope 5.10

Use the **`mac-host`** service, which `./4231.sh` picks by default on a Mac.
It needs Docker Desktop's **host networking**: *Settings → Resources → Network → Enable host networking*, then restart Docker Desktop.
Docker's internal subnet (192.168.65.0/24) is separate from the robot network, so leave it as it is.

1. Connect a USB/Thunderbolt Ethernet adapter to the robot. In *System Settings → Network*, set it to *Manually*: IP `192.168.0.77` (any free address), subnet `255.255.255.0`, no router.
2. In `setup/.env`, set `HOST_IP=192.168.0.77`.
   The container only sees Docker's VM addresses (192.168.65.x), so the driver has to be told the Mac's real IP (`reverse_ip`).
3. Enter the same IP as the URCap **Host IP** on the pendant.
4. Run `./4231.sh real`, **then** press Play on the pendant. If the program was already playing, press Stop and then Play. The URCap doesn't retry, so a program started before the driver fails with *"connection … timed out"*.
5. The driver terminal should print `Robot requested program` → `Sent program to robot` → `Robot connected to reverse interface`.

The desktop is still at <http://localhost:6080/vnc.html>, bound to localhost only.
Keep NordVPN or other VPNs off while using the robot.

The older `mac` service uses bridge networking with published ports (`MAC_SERVICE=mac ./4231.sh up`).
In testing, the robot's connections back to the Mac timed out and the driver's RTDE streams stalled after a few minutes, so only use it if host networking isn't available.

USB devices (RealSense, webcam, Arduino) **cannot** be passed into Docker Desktop on macOS.
Do camera or serial labs on a Linux PC, or record a rosbag there and replay it on the Mac.

### Manual launch (same commands as `scripts/`)

```bash
# terminal 1
ros2 launch ur_robot_driver ur_control.launch.py ur_type:=ur5e robot_ip:=192.168.0.100 \
    use_fake_hardware:=false launch_rviz:=false   # + reverse_ip:=$UR_REVERSE_IP on macOS
# terminal 2
ros2 launch ur_moveit_config ur_moveit.launch.py ur_type:=ur5e robot_ip:=192.168.0.100 launch_rviz:=true
```

The original `setupRealur5e.sh` and `setupFakeur5e.sh` open `gnome-terminal` windows, which isn't available in the container.
`ur5e_real.sh` and `ur5e_fake.sh` run the same two launches in a **tmux** session instead:

* `Ctrl-b n` / `Ctrl-b p`: switch between the driver and MoveIt windows
* `Ctrl-b d`: detach
* `tmux kill-session -t ur5e`: stop both

---

## Cameras and Arduino (Linux)

The `linux` service is `privileged` and mounts `/dev`, so devices plugged in after start-up still appear.

```bash
ros2 launch realsense2_camera rs_launch.py align_depth.enable:=true enable_color:=true \
    enable_depth:=true pointcloud.enable:=true                                            # lab 3
ros2 run v4l2_camera v4l2_camera_node --ros-args -p video_device:="/dev/video0"           # lab 4
ros2 run util_arduino_serial util_arduino_serial                                          # /dev/ttyUSB0
```

## Lab 4 (YOLO)

Clone `yolov8_ros` into `labs/lab4_workspace/src` and `pip install -r requirements.txt`, as the sheet says.
Use `pip install --user`: `~/.local` is kept in a Docker volume, but any other pip install is lost when the container is recreated.
If pip upgrades `numpy` to 2.x, `cv_bridge` breaks; fix it with `pip install --user "numpy<2"`.

`cheese` isn't in the image. To capture training images, use the host's camera app, or run `ros2 run image_view image_saver --ros-args -r image:=/image_raw`.

---

## Notes and troubleshooting

* **Apt vs supplied UR config:** the supplied `source/ros_ur_driver` has course edits that the apt `ur_moveit_config` doesn't include.
  For example, its SRDF adds a `test_configuration` goal state, which the Lab 3 sheet asks students to select in RViz.
  With apt, pick `home` or `up`, or drag the arm to a pose instead.
* **`ros_ur_driver` dependency:** `lab3_moveit` and `demo_moveit_ur` list `<depend>ros_ur_driver</depend>`, but that's a folder name, not a package.
  colcon only warns about it. You can remove the line from those `package.xml` files.
* **RViz on macOS** renders in software. It is usable, but slower than on a Linux PC with a GPU.
* **Linux: "cannot open display":** run `xhost +local:` on the host. `4231.sh` does this for you.
* **Linux: files owned by root/another UID:** `HOST_UID` and `HOST_GID` in `.env` must match `id -u` and `id -g`. Change them, then run `./4231.sh rebuild`.
* **Need another package?** Add it to the Dockerfile's apt list, then run `./4231.sh rebuild`.
* **Start again from a clean build:** delete the `build/ install/ log/` folders in the affected workspace, then run `./4231.sh build`.
* **Calibration warning:** the driver's "calibration parameters … don't match" error means the robot's factory calibration hasn't been extracted. Motion still works, but poses can be a few mm off.
  To fix it, run `ros2 launch ur_calibration calibration_correction.launch.py robot_ip:=192.168.0.100 target_filename:=$HOME/4231/ur5e_calibration.yaml`, then pass `kinematics_params_file:=...` to the driver.
* **Driver version:** the apt driver (`ros-humble-ur` 2.14) is newer than the supplied `source/ros_ur_driver` (2.2.8). It worked with PolyScope 5.10 and the lab's External Control URCap in testing.
