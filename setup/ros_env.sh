# Sourced by every interactive shell in the MTRN4231 container.
# Sources ROS 2 Humble (MoveIt + UR driver come from apt), then the course's
# shared workspaces once they have been built with `build_4231.sh`.

source /opt/ros/humble/setup.bash
[ -f /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash ] && \
    source /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash

COURSE_DIR="${COURSE_DIR:-$HOME/4231}"
for _ws in packages/4231_utils packages/4231_demo_packages; do
    if [ -f "$COURSE_DIR/$_ws/install/local_setup.bash" ]; then
        source "$COURSE_DIR/$_ws/install/local_setup.bash"
    fi
done
unset _ws

if [ ! -f "$COURSE_DIR/packages/4231_demo_packages/install/local_setup.bash" ] && [ -z "$_4231_WARNED" ]; then
    echo -e "\e[33m[4231] course packages not built yet — run: build_4231.sh\e[0m"
    export _4231_WARNED=1
fi
