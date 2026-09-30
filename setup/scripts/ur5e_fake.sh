#!/bin/bash
# Container version of scripts/setupFakeur5e.sh.
# Starts the UR driver (fake hardware) and MoveIt + RViz in a tmux session.
#   Ctrl-b then n / p  switch windows      Ctrl-b then d  detach
#   tmux kill-session -t ur5e              stop everything
set -e
SESSION=ur5e
tmux kill-session -t "$SESSION" 2>/dev/null || true

tmux new-session -d -s "$SESSION" -n driver \
  "bash -ic 'ros2 launch ur_robot_driver ur_control.launch.py ur_type:=ur5e robot_ip:=yyy.yyy.yyy.yyy \
     initial_joint_controller:=joint_trajectory_controller use_fake_hardware:=true launch_rviz:=false; exec bash'"
sleep 5
tmux new-window -t "$SESSION" -n moveit \
  "bash -ic 'ros2 launch ur_moveit_config ur_moveit.launch.py ur_type:=ur5e launch_rviz:=true use_fake_hardware:=true; exec bash'"

if [ -t 1 ]; then exec tmux attach -t "$SESSION"; else echo "[4231] Running in tmux session '$SESSION' — attach with: tmux attach -t $SESSION"; fi
