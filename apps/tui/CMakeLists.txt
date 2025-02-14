delameta_github_package(
    "ftxui:ArthurSonzogni/ftxui#v5.0.0"
    OPTIONS "FTXUI_ENABLE_INSTALL OFF"
)

file(GLOB_RECURSE SOURCES *.cpp)
add_executable(parkee-tui ${SOURCES})

target_link_libraries(parkee-tui PRIVATE
    Boost::preprocessor
    fmt-header-only
    ftxui::dom
    ftxui::component
    ftxui::screen
    parkee-interview
)

target_compile_options(parkee-tui PRIVATE
    -Wall
    -Wextra
)
