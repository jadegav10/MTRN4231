#!/bin/bash
# Container entrypoint. With ENABLE_VNC=1 it starts a full XFCE desktop in a
# virtual X server, reachable from the host at:
#   browser:     http://localhost:6080/vnc.html   (noVNC)
#   VNC client:  vnc://localhost:5901              (set VNC_PASSWORD for macOS Screen Sharing)
set -e

if [ "${ENABLE_VNC:-0}" = "1" ]; then
    export DISPLAY=:1
    rm -f /tmp/.X1-lock /tmp/.X11-unix/X1

    if [ -n "$VNC_PASSWORD" ]; then
        mkdir -p "$HOME/.vnc"
        echo "$VNC_PASSWORD" | vncpasswd -f > "$HOME/.vnc/passwd"
        chmod 600 "$HOME/.vnc/passwd"
        SECURITY=(-SecurityTypes VncAuth -PasswordFile "$HOME/.vnc/passwd")
    else
        SECURITY=(-SecurityTypes None)
    fi

    Xtigervnc :1 -geometry "${VNC_GEOMETRY:-1600x900}" -depth 24 \
        "${SECURITY[@]}" -localhost -rfbport 5901 -AlwaysShared \
        >/tmp/xvnc.log 2>&1 &
    for _ in $(seq 50); do [ -e /tmp/.X11-unix/X1 ] && break; sleep 0.1; done

    # XFCE session (panel, desktop icons, terminal). Terminals it opens are
    # interactive bash shells, so ROS 2 is already sourced in them.
    (cd "$HOME/4231" && dbus-launch --exit-with-session startxfce4 >/tmp/xfce.log 2>&1 &)

    websockify --web /opt/novnc "${NOVNC_LISTEN:-6080}" localhost:5901 >/tmp/novnc.log 2>&1 &
    echo "[4231] Desktop: http://localhost:6080/vnc.html?autoconnect=1&resize=remote  (or vnc://localhost:5901)"
fi

exec "$@"
