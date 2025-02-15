message(STATUS "Configuring app Qt6 app...")

set(CMAKE_AUTORCC ON)
find_package(Qt6
    COMPONENTS
        Core
        Gui
        Widgets
        SerialPort
        PrintSupport
    REQUIRED
)

# download qcustomplot from the official website
CPMAddPackage(
    NAME QCustomPlot
    URL  https://www.qcustomplot.com/release/2.1.1/QCustomPlot-source.tar.gz
    DOWNLOAD_ONLY YES
)

qt_standard_project_setup()

file(GLOB SOURCES *.cpp)
qt_add_executable(parkee-qt
    ${SOURCES}
    "${QCustomPlot_SOURCE_DIR}/qcustomplot.cpp"
    "icons/icons.qrc"
)

target_include_directories(parkee-qt PRIVATE
    "${QCustomPlot_SOURCE_DIR}"
)

target_link_libraries(parkee-qt PRIVATE
    Boost::preprocessor
    fmt-header-only
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::PrintSupport
    parkee-interview
)

target_compile_options(parkee-qt PRIVATE
    -Wall
    -Wextra
)

message(STATUS "Done")