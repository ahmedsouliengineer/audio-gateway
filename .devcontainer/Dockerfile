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
    lcov \
    # I am choosing to include 'entr' for the live-watching capability we discussed.
    entr \
    && rm -rf /var/lib/apt/lists/* && \
    cd /usr/src/googletest && \
    cmake . && \
    cmake --build . && \
    cp lib/*.a /usr/lib

RUN usermod -aG audio vscode
WORKDIR /app

# --- STAGE 2: Tester (Static Analysis & Coverage) ---
FROM developer AS tester
COPY . .

# STRATEGY: I am creating the LLVM-GCOV shim globally in the tester stage.
# This allows 'lcov' to talk to Clang 18 without version mismatches.
RUN echo '#!/bin/bash\nexec llvm-cov-18 gcov "$@"' > /usr/local/bin/llvm-gcov-18 && \
    chmod +x /usr/local/bin/llvm-gcov-18

# I am running cppcheck with specific suppressions for a cleaner build.
RUN cppcheck --enable=all --suppress=missingIncludeSystem src/ include/ --error-exitcode=1

# I am building with global coverage flags to resolve the 'undefined reference' linker errors.
RUN cmake -B build_cov \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=clang++-18 \
    -DENABLE_COVERAGE=ON && \
    cmake --build build_cov && \
    cd build_cov && \
    ctest --output-on-failure && \
    # I am choosing to specify the gcov-tool path to our newly created shim.
    lcov --capture \
         --directory . \
         --output-file main_coverage.info \
         --gcov-tool /usr/local/bin/llvm-gcov-18 && \
    lcov --remove main_coverage.info '/usr/*' '*/tests/*' '*/googletest/*' -o filtered.info

# --- STAGE 3: Builder (The Compilation Gate) ---
FROM developer AS builder
COPY . .

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

COPY --from=builder /app/build/AudioGateway .

ENTRYPOINT ["./AudioGateway"]