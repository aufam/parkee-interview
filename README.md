# Serial Data Visualizer

### GUI App with QT
![qt-demo](parkee-qt-demo.gif)
### Terminal UI with FTXUI
![tui-demo](parkee-tui-demo.gif)
### CLI Commands
![demo](parkee-demo.gif)

## Overview
The **Serial Data Visualizer** is a terminal-based application
that reads serial data from serial port, validates it using LRC,
and displays it in real-time using **Qt6** and **FTXUI**.
This project demonstrates efficient serial communication,
data handling, and visualization within a Linux environment.

## Table of Contents
- [Overview](#overview)
- [Table of Contents](#table-of-contents)
- [Project Tree](#project-tree)
- [Features](#features)
- [Technologies Used](#technologies-used)
- [rerequisites](#prerequisites)
- [Setup Pseudo Terminal for Simulation](#setup-pseudo-terminal-for-simulation)
- [Running the Application](#running-the-application)
- [Build](#build)
- [Testing](#testing)
- [Design Decisions](#design-decisions)

## Project Tree
```
.
├── apps/                 # Apps sourc code
│  ├── qt/
│  ├── receiver/
│  ├── sender/
│  ├── tui/
├── cmake/                # CMake helper
├── include/              # Public header directory 
│  ├── parkee-interview/
├── src/                  # Lib source code
├── test/                 # Testing source code
├── CMakeLists.txt        # Build configurations
├── Dockerfile            # Docker script
├── Makefile              # Build script
├── parkee-qt             # Compiled Qt app (compiled with GCC 13.3.0 in Ubuntu 24.04)
├── parkee-receiver       # Statically compiled CLI receiver
├── parkee-sender         # Statically compiled CLI sender
├── parkee-tui            # Statically compiled TUI app
├── parkee-demo.gif       # CLI demo
├── parkee-qt-demo.gif    # Qt app demo
├── parkee-tui-demo.gif   # TUI app demo
└── README.md             # Project doc
```

## Features
- **Serial Communication**: Reads and processes serial data streams.
- **Data Integrity Check**: Implements **LRC checksum** verification.
- **Real-Time Visualization**: Uses **Qt6** and **FTXUI** for the graphical interfaces.
- **Data Analysis**: Displays **min, max, and average** values.
- **Command Controls**:
  - **Start Data Collection**
  - **Stop Data Collection**
  - **View Raw Data** (shows received packets and warnings for invalid data)
  - **Clear Display** (resets visualization)

## Technologies Used
- **C++20**
- [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) (Dependency Management)
- [Delameta](https://github.com/aufam/delameta) (My own serial & socket framework, managed by CPM.cmake)
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) (Terminal UI, managed by CPM.cmake)
- **Qt6** (Must be installed on the target machine)
- [QCustomPlot](https://www.qcustomplot.com) (Plotting and Data visualization, managed by CPM.cmake)
- [Catch2](https://github.com/catchorg/Catch2) (Testing Framework, managed by CPM.cmake)

## Prerequisites
Ensure you have the following installed:
- **CMake 3.14+**
- **GCC/Clang with C++20 support**
- **Git**
- **Socat** for simulating serial port
  ```sh
  sudo apt install socat
  ```
- **Qt6** with **Qt6SerialPort**, **Qt6PrintSupport**
  ```sh
  sudo apt install qt6-base-dev qt6-serialport-dev qt6-printsupport-dev 
  ```
- **OpenGL** Libraries: **libGL1**, **libEGL1**, **libOpenGL0**
- System Libraries: **libstdc++6**, **libgcc1**, **libudev1**, **libfontconfig1**, **libxkbcommon0**

## Setup Pseudo Terminal for Simulation
Run the following command:
```sh
socat -d -d pty,raw,echo=0 pty,raw,echo=0
```
Or
```sh
make simulation-setup
```

This command creates two pseudo-terminals (PTYs) and links them together.
Any data sent to one PTY is forwarded to the other, effectively simulating a virtual serial connection.

When you run the command, you'll see output like:
```
PTY is /dev/pts/3
PTY is /dev/pts/4
```

This means `/dev/pts/3` and `/dev/pts/4` are now linked, and you can use them as a virtual serial port pair.

## Running the Application
In one terminal, run the Qt App with the following command:
```
./parkee-qt
```
Or, Terminal UI App with the forllowing command:
```sh
./parkee-tui
```
Make sure to input the correct serial port, for example, `/dev/pts/3` (see the demo animations above)

In another terminal, run the following commands to send data:
```sh
./parkee-sender --ports="/dev/pts/4" --value=0.5 # sending value 0.5
./parkee-sender --ports="/dev/pts/4" --random # sending random value
./parkee-sender --ports="/dev/pts/4" --random --broken # sending broken frame
```

For a complete guide you can see the demo animations above

## Build

### Clone
```sh
git clone https://github.com/aufam/parkee-interview.git
cd parkee-interview
```

### Configure and Build
```sh
cmake -B build -DCMAKE_CXX_STANDARD=20 -DPARKEE_BUILD_QT=ON # for Qt App
cmake -B build -DCMAKE_CXX_STANDARD=20 -DPARKEE_BUILD_TUI=ON # for Terminal UI App
cmake --build build
```
Or
```sh
make configure-qt
make configure-tui
make
```
The compiled executables for `parkee-sender`, `parkee-receiver`, `parkee-qt` and `parkee-tui` would be in `./build/apps/sender/parkee-sender`, `./build/apps/sender/parkee-receiver`, `./build/apps/qt/parkee-qt` and `./build/apps/tui/parkee-tui` respectively.

### Build with Docker
```
docker build -t parkee-interview .
```

## Testing
The project includes unit tests using **Catch2**:

### Configure and build
```sh
cmake -B build -DCMAKE_CXX_STANDARD=20 -DPARKEE_BUILD_TESTS=ON
cmake --build build
```
Or
```sh
make configure-tests
make
```

### Run
```sh
ctest --test-dir build/tests/ --output-on-failure
```
Or
```sh
make run-tests
```

## Design Decisions
- **FTXUI**: Lightweight terminal-based UI, and it can be statically linked.
- **Delameta Framework**: Used for modular and efficient serial communication.
- **CPM.cmake**: Simplifies dependency management.


## Credits
- [QCustomPlot](https://www.qcustomplot.com) 
- [Icons](https://github.com/ilya-sotnikov/qUART/tree/main/icons) 