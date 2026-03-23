# --- STAGE 1: Development & Build Environment ---
FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04 AS developer

# I am installing the industrial essentials, now including libzmq and boost.
# I am also building GTest from source to ensure binary compatibility with our C++23 build.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libasound2-dev \
    libgtest-dev \
    libzmq3-dev \
    libboost-all-dev \
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
    && rm -rf /var/lib/apt/lists/* && \
    cd /usr/src/googletest && \
    cmake . && \
    cmake --build . && \
    cp lib/*.a /usr/lib

RUN usermod -aG audio vscode
WORKDIR /app

# --- STAGE 2: Builder (The Compilation Gate) ---
FROM developer AS builder

# I am copying ONLY the exact files and folders needed for the build.
# This ensures a hermetic build and prevents local host "pollution."
COPY CMakeLists.txt .
COPY .clang-format .
COPY .clang-tidy .
COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/

# I am building the project using Clang for fuzzing compatibility.
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++-18 \
    -DENABLE_FUZZING=ON && \
    cmake --build build --target AudioGateway

# --- STAGE 3: Release (The Production Image) ---
FROM ubuntu:24.04 AS release

# I am installing only the runtime libraries (libasound2t64, libzmq5, boost-system).
# I am choosing these specific packages to keep the production image minimal.
RUN apt-get update && apt-get install -y --no-install-recommends \
    libasound2t64 \
    libzmq5 \
    libboost-system1.83.0 \
    alsa-utils \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m gateway_service && usermod -aG audio gateway_service
USER gateway_service
WORKDIR /app

# I am pulling ONLY the specific compiled binary from the builder.
COPY --from=builder /app/build/AudioGateway .

ENTRYPOINT ["./AudioGateway"]