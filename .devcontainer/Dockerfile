# --- STAGE 1: Development & Build Environment ---
FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04 AS developer

# I am adding lcov to the base so it is available for the tester stage.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libasound2-dev \
    libgtest-dev \
    libzmq3-dev \
    cppzmq-dev \
    libboost-all-dev \
    pkg-config \
    cmake \
    g++ \
    alsa-utils \
    clang-tidy \
    clang-format \
    gdb \
    cppcheck \
    clang-18 \
    llvm-18 \
    libfuzzer-18-dev \
    lcov \
    && rm -rf /var/lib/apt/lists/* && \
    cd /usr/src/googletest && \
    cmake . && \
    cmake --build . && \
    cp lib/*.a /usr/lib

RUN usermod -aG audio vscode
WORKDIR /app

# --- STAGE 2: Tester (Static Analysis & Coverage) ---
FROM developer AS tester
# I am choosing to copy only what is needed for analysis and testing.
COPY CMakeLists.txt .
COPY .clang-format .
COPY .clang-tidy .
COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/

# I am running cppcheck as the first gate.
RUN cppcheck --enable=all --suppress=missingIncludeSystem src/ include/ --error-exitcode=1

# I am building with coverage enabled.
RUN cmake -B build_cov \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=clang++-18 \
    -DENABLE_COVERAGE=ON && \
    cmake --build build_cov && \
    cd build_cov && ctest --output-on-failure && \
    lcov --capture --directory . --output-file main_coverage.info && \
    lcov --remove main_coverage.info '/usr/*' '*/tests/*' '*/googletest/*' -o filtered.info

# --- STAGE 3: Builder (The Compilation Gate) ---
FROM developer AS builder
# I am repeating the explicit copies to maintain layer independence.
COPY CMakeLists.txt .
COPY include/ ./include/
COPY src/ ./src/

# I am performing the final optimized Release build.
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++-18 \
    -DENABLE_FUZZING=ON && \
    cmake --build build --target AudioGateway

# --- STAGE 4: Release (The Production Image) ---
FROM ubuntu:24.04 AS release
RUN apt-get update && apt-get install -y --no-install-recommends \
    libasound2t64 \
    libzmq5 \
    libboost-system1.83.0 \
    alsa-utils \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m gateway_service && usermod -aG audio gateway_service
USER gateway_service
WORKDIR /app

# I am only pulling the final binary from the builder stage.
COPY --from=builder /app/build/AudioGateway .

ENTRYPOINT ["./AudioGateway"]