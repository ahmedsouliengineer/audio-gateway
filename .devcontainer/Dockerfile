# --- STAGE 1: The Base ---
FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04 AS base
# Added libzmq and boost dependencies for the monolith
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libasound2-dev \
    libzmq3-dev \
    libboost-all-dev \
    cmake \
    clang \
    ccache \
    cppcheck \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app

# --- STAGE 2: The Audit ---
FROM base AS audit
COPY . .
RUN --mount=type=cache,target=/root/.ccache \
    cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && \
    cppcheck --project=build/compile_commands.json --enable=all --error-exitcode=1 -i build/_deps

# --- STAGE 3: The Builder ---
FROM base AS builder
COPY --from=audit /app .
RUN --mount=type=cache,target=/root/.ccache \
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER_LAUNCHER=ccache && \
    cmake --build build --config Release

# --- STAGE 4: The Runtime ---
FROM ubuntu:24.04 AS runtime
# We only need the runtime libraries here, not the -dev headers
RUN apt-get update && apt-get install -y libasound2 libzmq5 alsa-utils && rm -rf /var/lib/apt/lists/*
RUN useradd -m gatewayuser && usermod -aG audio gatewayuser
USER gatewayuser
WORKDIR /app
# Ensure this matches the executable name in your CMakeLists.txt
COPY --from=builder /app/build/AudioGateway .
ENTRYPOINT ["./AudioGateway"]