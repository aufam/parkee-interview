delameta_github_package("Catch2:catchorg/Catch2#v3.7.0")

file(GLOB_RECURSE SOURCES *)
add_executable(test_all ${SOURCES})

target_link_libraries(test_all PRIVATE
    Boost::preprocessor
    fmt-header-only
    Catch2WithMain
    parkee-interview
)

target_compile_options(test_all PRIVATE
    -Wall
    -Wextra
)

enable_testing()
add_test(NAME test_all COMMAND test_all)

