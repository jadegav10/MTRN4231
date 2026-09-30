#!/bin/bash
# Build the course's shared workspaces: packages/4231_utils and packages/4231_demo_packages.
# MoveIt 2 and the UR driver come from apt (ros-humble-ur), so source/ws_moveit2 and
# source/ros_ur_driver are NOT built.
# Build output (build/ install/ log/) lands in each workspace inside the
# bind-mounted supplied-code folder.
#
# Usage: build_4231.sh [workspace ...]      (default: both)
set -eo pipefail

COURSE_DIR="${COURSE_DIR:-$HOME/4231}"
WORKSPACES="${*:-packages/4231_utils packages/4231_demo_packages}"
source /opt/ros/humble/setup.bash

for ws in $WORKSPACES; do
    echo -e "\n\e[1;36m[4231] ===== Building $ws =====\e[0m"
    cd "$COURSE_DIR/$ws"
    colcon build
    source install/local_setup.bash
done

echo -e "\n\e[1;32m[4231] Build complete. Open a new shell (or 'source /opt/4231/ros_env.sh').\e[0m"
