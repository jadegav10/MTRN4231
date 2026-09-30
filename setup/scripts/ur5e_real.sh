#!/bin/bash
# Container version of scripts/setupRealur5e.sh.
# Starts the UR driver against a physical UR5e plus MoveIt + RViz in tmux.
#
# Usage: ur5e_real.sh [ROBOT_IP]          (default 192.168.0.100)
# Env:   UR_REVERSE_IP  IP of *this computer* as seen by the robot. Needed when
#                       the container is not on host networking (macOS) —
#                       compose sets it from HOST_IP. Leave empty on Linux.
#
# On the teach pendant: run a program containing the External Control URCap,
# whose "Host IP" is this computer's IP on the robot network.
set -e
ROBOT_IP="${1:-${ROBOT_IP:-192.168.0.100}}"
SESSION=ur5e
REVERSE_ARG=""
[ -n "$UR_REVERSE_IP" ] && REVERSE_ARG="reverse_ip:=$UR_REVERSE_IP"

echo "[4231] Robot IP: $ROBOT_IP   reverse IP: ${UR_REVERSE_IP:-auto}"
if ! ping -c1 -W2 "$ROBOT_IP" >/dev/null 2>&1; then
    echo -e "\e[33m[4231] WARNING: cannot ping $ROBOT_IP — check cable / IP settings.\e[0m"
fi

tmux kill-session -t "$SESSION" 2>/dev/null || true
tmux new-session -d -s "$SESSION" -n driver \
  "bash -ic 'ros2 launch ur_robot_driver ur_control.launch.py ur_type:=ur5e robot_ip:=$ROBOT_IP \
     use_fake_hardware:=false launch_rviz:=false $REVERSE_ARG; exec bash'"
sleep 10
tmux new-window -t "$SESSION" -n moveit \
  "bash -ic 'ros2 launch ur_moveit_config ur_moveit.launch.py robot_ip:=$ROBOT_IP ur_type:=ur5e launch_rviz:=true; exec bash'"

if [ -t 1 ]; then exec tmux attach -t "$SESSION"; else echo "[4231] Running in tmux session '$SESSION' — attach with: tmux attach -t $SESSION"; fi
