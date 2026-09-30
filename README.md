# MTRN4231

Course code for MTRN4231 (UNSW): demo packages, utilities, lab workspaces, and a Docker environment that runs all of it (ROS 2 Humble + UR5e driver + MoveIt) on a Mac or a Linux PC.

## Get the code

```bash
git clone https://github.com/jadegav10/MTRN4231.git ~/4231
```

The labs refer to `~/4231/...`. Clone the repo there on Ubuntu; on a Mac, anywhere is fine.

To pull in updates later: `cd ~/4231 && git pull`.
Keep your own work in the lab workspaces or your own packages. Don't edit the demo packages, so `git pull` never conflicts.

## Option A: Ubuntu 22.04 + ROS 2 Humble (lab machines, dual boot)

Follow the course Install Guide on Moodle, then:

```bash
sudo apt install ros-humble-ur python3-colcon-common-extensions
```

## Option B: Docker (macOS or Linux)

This gives you the whole environment in a container, including a full Linux desktop in your browser.

1. Install [Docker Desktop](https://www.docker.com/products/docker-desktop/) (Mac) or Docker Engine (Linux).
   **Mac:** also turn on *Settings → Resources → Network → Enable host networking*, then restart Docker Desktop.
2. Run:
    ```bash
    cd ~/4231/docker
    ./4231.sh up        # first run builds the image: 15–30 min
    ./4231.sh build     # builds 4231_utils + 4231_demo_packages: ~1 min
    ```
3. **Mac:** open the desktop at <http://localhost:6080/vnc.html?autoconnect=1&resize=remote>.
   **Linux:** windows open on your own screen.
4. Try the simulated arm: double-click **UR5e (fake hardware)** on the desktop, or run `./4231.sh fake`.

Settings (robot IP, your IP on the robot network, `ROS_DOMAIN_ID`, VNC password) live in `docker/.env`, which is created on first run.
[`docker/README.md`](docker/README.md) covers connecting to the real UR5e, cameras, and troubleshooting.

---

# Course overview
 

## File Overview

Welcome to 4231, hope you enjoy this course and learning ROS. To help you in your learning some demo, utility and stub files are provided. An overview of the included directories can be found below. 

### 4231_demo_packages

* demo_client_service 
    - client example
    - service example

* demo_interactive_marker
    - basic interactive commands

* demo_lauch_param
    - rviz marker example
    - launch file example

* demo_moveit_ur
    - basic movement example
    - collision avoid example
    - constained path example
    - servo movement example

* demo_tf2
    - static frame example
    - dyanmic frame example
    - listner example


### 4231_scripts

* camera.sh
    - Launch ros2 camera interface

* setupFakeur5e.sh
    - Launches ur driver and moveit server 
    - Launches in visulisation mode
    - Launches rviz with motion planner interface

* setupRealur5e.sh
    - Launches ur driver and moveit server 
    - Connects to physical hardware and IP address
    - Launches rviz with motion planner interface

### 4231_utils

* util_arduino_serial 
    - Creates a serial communication with arduino, passes through string message from a topic

* util_keyboard 
    - Allows a user to publish key presses to a topic


### Labs and Project Workspace
These directories allow for lab and project work. Lab instructions can be found inside each directory. 


### ros_ur_driver
Source install of the ros universal robotics driver. Has been source installed as some files have been changed for course convience. 

*Edit at your own risk*

If rebuilding follow instructions on github carefully.


### ws_moveit2
Source install of moveit 2. Has been source installed as some files have been changed for course convience.

*Edit at your own risk*

If rebuilding follow instruction on github carefully. A full rebuild can take 30+ minutes.