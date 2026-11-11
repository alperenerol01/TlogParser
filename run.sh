#!/bin/sh
set -e

cd "$(dirname "$0")"

docker build -t tlog-parser .

if [ -d /mnt/wslg ]; then
    docker run --rm \
        -e DISPLAY=:0 \
        -v /tmp/.X11-unix:/tmp/.X11-unix \
        -v /mnt/wslg:/mnt/wslg \
        tlog-parser
elif [ -n "$DISPLAY" ]; then
    xhost +local:docker > /dev/null 2>&1 || true
    docker run --rm \
        -e DISPLAY="$DISPLAY" \
        -v /tmp/.X11-unix:/tmp/.X11-unix \
        tlog-parser
else
    echo "No graphical display was found."
    echo "The application was built at /src/build/bin/TlogParser inside the tlog-parser image."
fi
