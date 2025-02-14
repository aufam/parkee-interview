cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# project settings
project(parkee-interview
    VERSION     0.1.0
    DESCRIPTION "Parkee Interview Question: Serial Data Visualizer"
)

# some options
option(PARKEE_BUILD_TUI     "Build Terminal UI app with FTXUI"  OFF)
option(PARKEE_BUILD_TESTS   "Generate test cases"               OFF)

# some messages
message(STATUS "PARKEE_BUILD_TUI   : ${PARKEE_BUILD_TUI}")
message(STATUS "PARKEE_BUILD_TESTS : ${PARKEE_BUILD_TESTS}")

# external libraries
include("cmake/delameta.cmake")
## advance preprocessor
delameta_github_package("preprocessor:boostorg/preprocessor#boost-1.86.0")
## string formatter
delameta_github_package("fmt:fmtlib/fmt#11.0.2" OPTIONS "FMT_INSTALL OFF")
## socket and serial framework
delameta_github_package("delameta:aufam/delameta#main" OPTIONS "DELAMETA_INSTALL OFF")

# lib
file(GLOB_RECURSE SOURCES src/*)
add_library(parkee-interview ${SOURCES})

target_compile_definitions(parkee-interview PUBLIC
    -DPARKEE_PROJECT_NAME="${CMAKE_PROJECT_NAME}"
    -DPARKEE_PROJECT_VERSION="${parkee-interview_VERSION}"
)

target_include_directories(parkee-interview PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(parkee-interview
    PRIVATE
        Boost::preprocessor
        fmt-header-only
    PUBLIC
        delameta
)

target_compile_options(parkee-interview PRIVATE
    -Wall
    -Wextra
)

# sender/receiver CLI app
add_subdirectory("apps/sender")
add_subdirectory("apps/receiver")

# apps and tests
if (PARKEE_BUILD_TUI)
    add_subdirectory("apps/tui")
endif()
if (PARKEE_BUILD_TESTS)
    add_subdirectory("tests")
endif()
