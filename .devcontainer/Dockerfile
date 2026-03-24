# --- STAGE 1: Development & Build Environment ---
FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04 AS developer

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
    # I am removing lcov as it is no longer needed for Clang 18 native coverage.
    && rm -rf /var/lib/apt/lists/* && \
    cd /usr/src/googletest && \
    cmake . && \
    cmake --build . && \
    cp lib/*.a /usr/lib

RUN usermod -aG audio vscode
WORKDIR /app

# --- STAGE 2: Tester (Static Analysis & Native Coverage) ---
FROM developer AS tester
COPY . .

# I am running cppcheck as the first gate.
RUN cppcheck --enable=all --suppress=missingIncludeSystem src/ include/ --error-exitcode=1

# I am building and running tests with native LLVM instrumentation.
RUN cmake -B build_cov \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=clang++-18 \
    -DENABLE_COVERAGE=ON && \
    cmake --build build_cov && \
    cd build_cov && \
    ctest --output-on-failure && \
    # I am merging the raw profile data into a format llvm-cov can read.
    llvm-profdata-18 merge -sparse default.profraw -o test.profdata && \
    # I am choosing to output a summary report to the build logs.
    llvm-cov-18 report ./unit_tests -instr-profile=test.profdata src/

# --- STAGE 3: Builder (The Compilation Gate) ---
FROM developer AS builder
COPY . .

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

COPY --from=builder /app/build/AudioGateway .

ENTRYPOINT ["./AudioGateway"]