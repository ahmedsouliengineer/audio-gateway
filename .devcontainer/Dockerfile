# --- STAGE 1: Development & Build Environment ---
FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04 AS developer

# I am installing the industrial essentials. 
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libasound2-dev \
    libgtest-dev \
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
    && rm -rf /var/lib/apt/lists/*

RUN usermod -aG audio vscode
WORKDIR /app

# --- STAGE 2: Builder (The Compilation Gate) ---
FROM developer AS builder

# I am copying ONLY the structural, source, and configuration files.
# I am including the .clang-format and .clang-tidy files so the 
# builder stage can enforce our coding standards during the audit.
COPY CMakeLists.txt .
COPY .clang-format .
COPY .clang-tidy .
COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/

# I am building the project.
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --target AudioGateway

# --- STAGE 3: Release (The Production Image) ---
FROM ubuntu:24.04 AS release
RUN apt-get update && apt-get install -y --no-install-recommends \
    libasound2t64 \
    alsa-utils \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m gateway_service && usermod -aG audio gateway_service
USER gateway_service
WORKDIR /app

# I am pulling ONLY the specific compiled binary.
COPY --from=builder /app/build/AudioGateway .

ENTRYPOINT ["./AudioGateway"]