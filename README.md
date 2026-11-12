# TLOG Parser

A desktop application for opening and inspecting the MAVLink telemetry logs (`.tlog`). Built with C++17 and Qt 6 (QML).

When you start the application, the only thing you see is a file loading screen. Once you pick a `.tlog` file, it switches to a viewer with two tabs:

- **Map:** the flight path is drawn on the map and a marker shows where the vehicle is at that moment. Latitude, longitude and altitude are shown in the top left corner.
- **Raw Data:** every MAVLink message in the log is listed in a readable form, with its time, name and fields.

The timeline at the bottom of the window is shared by both tabs. When you drag it or play the recording, the marker on the map and the highlighted row in the list move together.

## Running

The project is built inside Docker, so you do not need Qt on your machine. The only requirements are Git and Docker.

Clone the repository first.

```bash
git clone --recursive https://github.com/alperenerol01/TlogParser.git
cd TlogParser
```

### With run.sh

```bash
./run.sh
```

It builds the Docker image first and then starts the application.

### Without run.sh

To do it manually, follow these two steps.

To build the image:

```bash
docker build -t tlog-parser .
```

This command builds the project and runs the tests. If any test fails, the image is not created. When the build finishes, the executable is at this path inside the image:

```
/src/build/bin/TlogParser
```

Then start the application. The container needs access to your display to open a window, so the command depends on your environment.

On Windows, from inside WSL 2:

```bash
docker run --rm -e DISPLAY=:0 -v /tmp/.X11-unix:/tmp/.X11-unix -v /mnt/wslg:/mnt/wslg tlog-parser
```

On Linux:

```bash
xhost +local:docker
docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix tlog-parser
```

## Project structure

The code is split into two layers. `core` only reads and decodes the log. `app` is the user interface.

```
src/
├── core/                   Log reading and decoding
│   ├── MavlinkFrameParser      Extracts a MAVLink frame
│   ├── TlogReader              Reads the file, creates the packets
│   └── MavlinkDecoder          Turns a packet into readable text and a position
└── app/                    User interface (Qt Quick)
    ├── TimeFormat              Formats a duration as mm:ss
    ├── models/
    │   └── PacketListModel     QAbstractListModel that feeds the raw data list
    ├── controllers/
    │   ├── LogSession          Loads a file
    │   └── PlaybackController  Timeline position, playback
    └── views/
        ├── pages/              LoadPage, ViewerPage, MapTab, RawDataTab
        └── components/         PacketRow, TimelineBar, PageTab, PlayerButton
```
