FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=C.UTF-8

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake ninja-build git python3 python3-venv \
        ca-certificates libssl3t64 \
        libgl1-mesa-dev libegl1 libglib2.0-0t64 libdbus-1-3 \
        libfontconfig1-dev libfreetype-dev fonts-dejavu-core \
        libxkbcommon-dev libxkbcommon-x11-0 libx11-xcb1 \
        libxcb-cursor0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-randr0 \
        libxcb-render-util0 libxcb-shape0 libxcb-xinerama0 libxcb-xkb1 \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m venv /opt/aqt \
    && /opt/aqt/bin/pip install --no-cache-dir aqtinstall \
    && /opt/aqt/bin/aqt install-qt linux desktop 6.9.3 linux_gcc_64 \
        -m qtlocation qtpositioning -O /opt/Qt \
    && rm -rf /opt/aqt

ENV CMAKE_PREFIX_PATH=/opt/Qt/6.9.3/gcc_64

WORKDIR /src
COPY . .

RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build \
    && ctest --test-dir build --output-on-failure

CMD ["./build/bin/TlogParser"]
