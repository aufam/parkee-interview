file(GLOB_RECURSE SOURCES *)
add_executable(parkee-sender ${SOURCES})

target_link_libraries(parkee-sender PRIVATE
    Boost::preprocessor
    fmt-header-only
    parkee-interview
)

target_compile_options(parkee-sender PRIVATE
    -Wall
    -Wextra
)
