file(GLOB_RECURSE SOURCES *)
add_executable(parkee-receiver ${SOURCES})

target_link_libraries(parkee-receiver PRIVATE
    Boost::preprocessor
    fmt-header-only
    parkee-interview
)

target_compile_options(parkee-receiver PRIVATE
    -Wall
    -Wextra
)
