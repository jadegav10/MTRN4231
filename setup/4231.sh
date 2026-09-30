#!/bin/bash
# MTRN4231 container helper. Picks the right compose service for this OS.
#
#   ./4231.sh up        build (first time) and start the container
#   ./4231.sh shell     open a shell in the container (run as many as you like)
#   ./4231.sh build     colcon-build packages/4231_utils + packages/4231_demo_packages
#   ./4231.sh fake      start the fake UR5e + MoveIt + RViz
#   ./4231.sh real [IP] connect to the physical UR5e (default 192.168.0.100)
#   ./4231.sh desktop   open the noVNC desktop in a browser (macOS)
#   ./4231.sh stop      stop the container       ./4231.sh down   remove it
#   ./4231.sh rebuild   rebuild the image (after editing the Dockerfile)
set -e
cd "$(dirname "$0")"

case "$(uname -s)" in
    Darwin) SERVICE="${MAC_SERVICE:-mac-host}" ;;   # MAC_SERVICE=mac for port-forwarding fallback
    *)      SERVICE=linux ;;
esac

if [ ! -f .env ]; then
    cp .env.example .env
    if [ "$SERVICE" = linux ]; then
        sed -i "s/^HOST_UID=.*/HOST_UID=$(id -u)/; s/^HOST_GID=.*/HOST_GID=$(id -g)/" .env
    fi
    echo "[4231] Created .env — review it (ROBOT_IP / HOST_IP / ROS_DOMAIN_ID)."
fi

running() { [ "$(docker inspect -f '{{.State.Running}}' mtrn4231 2>/dev/null)" = "true" ]; }
compose_up() {
    # A container called mtrn4231 started from a different folder blocks ours.
    other="$(docker inspect -f '{{index .Config.Labels "com.docker.compose.project.working_dir"}}' mtrn4231 2>/dev/null || true)"
    if [ -n "$other" ] && [ "$other" != "$PWD" ]; then
        echo "[4231] A container named 'mtrn4231' already exists, started from: $other"
        echo "[4231] Remove it (your code is not affected) and try again:  docker rm -f mtrn4231"
        exit 1
    fi
    # Build locally when the image is missing; never try to pull it from Docker Hub.
    docker image inspect mtrn4231:humble >/dev/null 2>&1 || docker compose build "$SERVICE"
    [ "$SERVICE" = linux ] && command -v xhost >/dev/null && xhost +local: >/dev/null
    docker compose up -d --pull never "$SERVICE"   # recreates the container if .env / compose changed
}
ensure_up() { running || compose_up; }
in_container() { ensure_up; docker exec -it mtrn4231 bash -ic "$*"; }

cmd="${1:-shell}"; shift || true
case "$cmd" in
    up)      compose_up
             [ "$SERVICE" != linux ] && echo "[4231] GUI desktop: http://localhost:6080/vnc.html?autoconnect=1&resize=remote" ;;
    shell)   ensure_up; docker exec -it mtrn4231 bash ;;
    build)   in_container build_4231.sh "$@" ;;
    fake)    in_container ur5e_fake.sh ;;
    real)    in_container ur5e_real.sh "$@" ;;
    desktop) open "http://localhost:6080/vnc.html?autoconnect=1&resize=remote" 2>/dev/null \
               || xdg-open "http://localhost:6080/vnc.html?autoconnect=1&resize=remote" ;;
    stop)    docker compose stop ;;
    down)    docker compose down ;;
    rebuild) docker compose build "$SERVICE" && docker compose up -d --pull never --force-recreate "$SERVICE" ;;
    *)       sed -n '2,12p' "$0"; exit 1 ;;
esac
