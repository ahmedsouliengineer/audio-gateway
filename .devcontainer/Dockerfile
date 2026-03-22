# --- STAGE 1: The Base ---
FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04 AS base
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libc++-dev \
    libc++abi-dev \
    libasound2-dev \
    libzmq3-dev \
    libboost-all-dev \
    libgtest-dev \
    cmake \
    clang \
    ccache \
    cppcheck \
    && rm -rf /var/lib/apt/lists/* && \
    cd /usr/src/googletest && \
    cmake . && \
    cmake --build . && \
    cp lib/*.a /usr/lib
WORKDIR /app

# --- STAGE 2: The Audit ---
FROM base AS audit
COPY . .
RUN --mount=type=cache,target=/root/.ccache \
    cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && \
    cppcheck --project=build/compile_commands.json \
             --enable=all \
             --suppress=missingIncludeSystem \
             --error-exitcode=1 \
             -i build/_deps

# --- STAGE 3: The Builder ---
FROM base AS builder
COPY --from=audit /app .
# FIX: Use Clang for the builder to ensure Fuzzing compatibility
# FIX: Added -DENABLE_FUZZING=ON so the CI can find the fuzzer binary later
RUN --mount=type=cache,target=/root/.ccache \
    cmake -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_COMPILER=clang++ \
        -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
        -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++" \
        -DENABLE_FUZZING=ON \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache && \
    cmake --build build --config Release

# --- STAGE 4: The Runtime ---
FROM ubuntu:24.04 AS runtime
# FIX: Added libboost-thread1.83.0 (or similar) if needed, 
# though lockfree is header-only, it's safer to have the base boost libs.
RUN apt-get update && apt-get install -y \
    libasound2 \
    libzmq5 \
    libboost-system1.83.0 \
    alsa-utils \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m gatewayuser && usermod -aG audio gatewayuser
USER gatewayuser
WORKDIR /app

# FIX: Verified matching the project name 'AudioGateway' from CMake
COPY --from=builder /app/build/AudioGateway .

# If you built multiple targets (like unit_tests), you might want to copy them 
# if you plan to run them in this stage, but usually, we just need the main app.
ENTRYPOINT ["./AudioGateway"]