all: build

configure-tui:
	cmake -B build -DCMAKE_CXX_STANDARD=20 -DPARKEE_BUILD_TUI=ON

configure-tests:
	cmake -B build -DCMAKE_CXX_STANDARD=20 -DPARKEE_BUILD_TESTS=ON

build:
	cmake --build build/

run-tests:
	ctest --test-dir build/tests/ --output-on-failure

simulation-setup:
	socat -d -d pty,raw,echo=0 pty,raw,echo=0
