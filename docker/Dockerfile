# MTRN4231 development image — ROS 2 Humble on Ubuntu 22.04 (amd64 + arm64)
#
# Only the *dependencies* are baked into the image. The course code itself is
# bind-mounted at ~/4231 at run time, so anything built with colcon ends up
# in the supplied-code folder on the host.
#
# Follows the MTRN4231 Install Guide: Ubuntu 22.04 + ROS 2 Humble *desktop*,
# the guide's "useful installs", and the UR driver + MoveIt from apt
# (ros-humble-ur). MoveIt is NOT built from the supplied source.

FROM ros:humble

ARG USERNAME=student
ARG USER_UID=1000
ARG USER_GID=1000
ARG DEBIAN_FRONTEND=noninteractive

# ---------------------------------------------------------------------------
# Base desktop, tooling, hardware drivers and GUI helpers
# ---------------------------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
        ros-humble-desktop \
        ros-humble-rqt* \
        ros-humble-xacro \
        ros-humble-joint-state-publisher-gui \
        ros-humble-ur-description \
        ros-humble-ur-client-library \
        ros-humble-ur-msgs \
        ros-humble-ros2-control \
        ros-humble-ros2-controllers \
        ros-humble-cv-bridge \
        ros-humble-image-transport-plugins \
        ros-humble-image-view \
        ros-humble-v4l2-camera \
        ros-humble-realsense2-camera \
        ros-humble-librealsense2 \
        ros-humble-rosbag2-storage-default-plugins \
        ros-humble-tf2-tools \
        python3-colcon-common-extensions \
        python3-rosdep python3-vcstool python3-pip \
        python3-serial python3-pygame \
        build-essential cmake git ccache gdb \
        sudo bash-completion nano vim less wget curl \
        iputils-ping iproute2 net-tools netcat-openbsd \
        tmux xterm usbutils v4l-utils \
        mesa-utils libgl1-mesa-dri \
        # virtual desktop served over noVNC (used on macOS / headless hosts)
        tigervnc-standalone-server openbox novnc python3-websockify \
    && rm -rf /var/lib/apt/lists/*

# ---------------------------------------------------------------------------
# Install Guide Task 4: Universal Robots ROS 2 driver (pulls in MoveIt 2 from
# apt), plus the apt packages the supplied demo/lab packages depend on.
# ---------------------------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
        ros-humble-ur \
        ros-humble-moveit \
        ros-humble-moveit-visual-tools \
        ros-humble-tf-transformations \
        ros-humble-interactive-markers \
        ros-humble-example-interfaces \
    && rm -rf /var/lib/apt/lists/*

# ---------------------------------------------------------------------------
# Install Guide Task 3: pip + opencv-python.
#   numpy is kept < 2 so the apt cv_bridge (built against numpy 1.x) works.
#   pyrealsense2 (lab 3) only has x86_64 wheels on PyPI — skipped on arm64.
# ---------------------------------------------------------------------------
RUN pip3 install --no-cache-dir "numpy<2" "opencv-python<4.11" transforms3d \
    && (pip3 install --no-cache-dir pyrealsense2 \
        || echo "WARNING: pyrealsense2 wheel not available for $(dpkg --print-architecture) — skipped")

# ---------------------------------------------------------------------------
# Current noVNC for the browser desktop (Ubuntu's 1.0 package crashes in
# vnc.html: "Cannot read properties of null (reading 'type')").
# ---------------------------------------------------------------------------
ARG NOVNC_VERSION=1.5.0
RUN curl -fsSL https://github.com/novnc/noVNC/archive/refs/tags/v${NOVNC_VERSION}.tar.gz \
      | tar -xz -C /opt && mv /opt/noVNC-${NOVNC_VERSION} /opt/novnc \
    && ln -s vnc.html /opt/novnc/index.html

# ---------------------------------------------------------------------------
# Full XFCE desktop for the VNC session (terminal, file manager, editor).
# ---------------------------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
        xfce4-session xfwm4 xfce4-panel xfdesktop4 xfce4-settings xfconf \
        xfce4-terminal thunar mousepad xfce4-appfinder \
        dbus-x11 at-spi2-core adwaita-icon-theme-full elementary-xfce-icon-theme \
        fonts-dejavu fonts-ubuntu tigervnc-tools \
    && rm -rf /var/lib/apt/lists/*

# ---------------------------------------------------------------------------
# Non-root user whose UID/GID match the host user (keeps bind-mount file
# ownership sane on Linux hosts).
# ---------------------------------------------------------------------------
RUN if getent passwd ${USER_UID} >/dev/null; then userdel -r "$(getent passwd ${USER_UID} | cut -d: -f1)"; fi \
    && (getent group ${USER_GID} >/dev/null || groupadd --gid ${USER_GID} ${USERNAME}) \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m -s /bin/bash ${USERNAME} \
    && usermod -aG dialout,video,plugdev ${USERNAME} \
    && install -d -o ${USER_UID} -g ${USER_GID} /home/${USERNAME}/.local /home/${USERNAME}/4231 \
    && echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME}

COPY ros_env.sh /opt/4231/ros_env.sh
COPY entrypoint.sh /opt/4231/entrypoint.sh
COPY scripts/ /opt/4231/scripts/
COPY desktop/ /opt/4231/desktop/
RUN chmod +x /opt/4231/entrypoint.sh /opt/4231/scripts/*.sh \
    && H=/home/${USERNAME} \
    && install -d -o ${USER_UID} -g ${USER_GID} $H/.config/xfce4/xfconf/xfce-perchannel-xml $H/Desktop \
    && cp /etc/xdg/xfce4/panel/default.xml $H/.config/xfce4/xfconf/xfce-perchannel-xml/xfce4-panel.xml \
    && cp /opt/4231/desktop/xfconf/*.xml $H/.config/xfce4/xfconf/xfce-perchannel-xml/ \
    && cp /opt/4231/desktop/*.desktop $H/Desktop/ && chmod +x $H/Desktop/*.desktop \
    && chown -R ${USER_UID}:${USER_GID} $H/.config $H/Desktop \
    && echo "source /opt/4231/ros_env.sh" >> /etc/bash.bashrc

ENV PATH="/opt/4231/scripts:${PATH}" \
    RCUTILS_COLORIZED_OUTPUT=1 \
    QT_X11_NO_MITSHM=1

USER ${USERNAME}
WORKDIR /home/${USERNAME}/4231

ENTRYPOINT ["/opt/4231/entrypoint.sh"]
CMD ["sleep", "infinity"]
