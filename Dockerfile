FROM alpine:3.20.3 AS builder

RUN apk add --no-cache \
    git \
    cmake \
    make \
    g++

WORKDIR /root/parkee

# copy cmake build
COPY cmake/ cmake/
COPY apps/sender/CMakeLists.txt apps/sender/
COPY apps/receiver/CMakeLists.txt apps/receiver/
COPY apps/tui/CMakeLists.txt apps/tui/
COPY CMakeLists.txt .


# configure and build ftxui
RUN mkdir -p apps/tui && echo "" > apps/tui/dummy.cpp && \
    mkdir -p apps/sender && echo "" > apps/sender/dummy.cpp && \
    mkdir -p apps/receiver && echo "" > apps/receiver/dummy.cpp && \
    mkdir -p src/ && echo "" > src/dummy.cpp && \
    cmake -B build \
    -DPARKEE_BUILD_TUI=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_EXE_LINKER_FLAGS="-static" && \
    cmake --build build -t screen dom component


# build lib
COPY include/ include/
COPY src/ src/

RUN cmake -B build && \
    cmake --build build -t parkee-interview


# build apps
COPY apps/tui/ apps/tui/
COPY apps/sender/ apps/sender/
COPY apps/receiver/ apps/receiver/

RUN cmake -B build && \
    cmake --build build -t parkee-sender parkee-receiver parkee-tui


# runner
FROM alpine:3.20.3 AS runner

WORKDIR /root/parkee

COPY --from=builder /root/parkee/build/apps/tui/parkee-tui /usr/bin
COPY --from=builder /root/parkee/build/apps/sender/parkee-sender /usr/bin
COPY --from=builder /root/parkee/build/apps/receiver/parkee-receiver /usr/bin

CMD ["parkee-tui"]
