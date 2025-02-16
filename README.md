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
### Data Structure
[parkee::Payload](./include/parkee-interview/payload.h)
has two fields which is `time_point timestamp` and `float value`.
The constructor is set to be private to enforce user to use Factory functions `Payload::New`, `Payload::Random`.

### Statistics
Average, max, and min values can be optained using static member functions
`Payload::get_average`, `Payload::get_max`, `Payload::get_min` respectively.
The values are updated each time `Payload` constructor is invoked.
The average value is calculated using moving average,
whilest min and max are calculated using comparison operation.

Here is the source code:
```c++
parkee::Payload::Payload(std::chrono::system_clock::time_point timestamp, float value)
    : timestamp(timestamp)
    , value(value)
{
    // moving average
    cnt++;
    average = ((average * (cnt - 1)) + value) / cnt;

    // max and min
    max = std::isnan(max) ? value : max > value ? max : value;
    min = std::isnan(min) ? value : min < value ? min : value;
}
```

### Serialization and Deserialization
`Payload::serialize` is a member function that converts `this Payload`
into `std::vector` with the given frame structures.

`Payload::deserialize` is a **static** member function that construct
`Payload` from `std::vector`. It returns `delameta::Result<Payload>`
which holds variant of `Payload` and `delameta::Error`.

This return-error-by-value paradigm is chosen over throw-catch to open the
possibility of this library to be used in embedded systems / microcontroller
where throwing exceptions are costly.

You can use the function like:
```c++
Result<Payload> payload_result = Payload::deserialize(raw_data);
if (payload_result.is_ok()) {
  Payload& payload = payload_result.unwrap();
  // do something
} else {
  Error& err = payload_result.unwrap_err();
  // do other thing
}
```

### LRC Calculation
Since the LRC is only one byte, simple XOR operation is implemented.
```c++
uint8_t parkee::lrc::encode(
  std::vector<uint8_t>::const_iterator begin,
  std::vector<uint8_t>::const_iterator end
) {
  uint8_t res = 0x00;
  for (; begin != end; ++begin) res ^= *begin;
  return res;
}
```

### Serial Communication
In QT App, QSerialPort is used to implement the
[SerialTransceiver](apps/qt/SerialTransceiver.cpp).

`QSerialPort::readyRead` is connected with `SerialTransceiver::receiveData`
as follows:
```c++
SerialTransceiver::SerialTransceiver(QObject *parent) : QSerialPort(parent) {
  connect(this, &QSerialPort::readyRead, this, &SerialTransceiver::receiveData);
}
```

The `SerialTransceiver::receivedData` reads all available data, deserialize it into
`Payload` and emit string message as well as payload's value if deserialization succeed
```c++
void SerialTransceiver::receiveData() {
  auto byteArray = readAll();
  auto raw = std::vector<uint8_t>(byteArray.size());
  std::memcpy(raw.data(), byteArray.constData(), byteArray.size());

  auto payload = Payload::deserialize(raw);

  emit emitMessage(QString::fromStdString(
    fmt::format("Raw: {:02X} {}", fmt::join(raw, " "), payload)
  ));

  if (payload.is_ok()) {
    emit emitNewValue(payload.unwrap().value);
  }
}
```

And then in the [MainWindow](apps/qt/mainwindow.cpp):
```c++
// connections
connect(serialTransceiver, &SerialTransceiver::emitNewValue, chart, &Chart::addData);
connect(serialTransceiver, &SerialTransceiver::emitMessage, this, [this](QString msg) {
  info->setText(msg);
  log.append(msg);
  updateStats();
});
```

In TUI App, `delameta::Serial` is used to handle serial communication.
We can do something like:
```c++
Result<Payload> payload_result = 
  Serial::Open(Serial::Args{.port=port, .baud=baud, .timeout=1})
  .and_then([](Serial serial) {
    return serial.read();
  })
  .and_then([](std::vector<uint8_t> raw_data) {
    return Payload::deserialize(raw_data);
  });
```

## Credits
- [QCustomPlot](https://www.qcustomplot.com) 
- [Icons](https://github.com/ilya-sotnikov/qUART/tree/main/icons) 