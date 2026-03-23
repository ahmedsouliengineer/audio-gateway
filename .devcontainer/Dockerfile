# I am choosing Ubuntu 24.04 to ensure we have GCC 13+ and Clang 18+ 
# available natively for C++23 support.
FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

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
    cppcheck

# I am adding the 'vscode' user to the 'audio' group to ensure 
# my application can open /dev/snd without root permissions.
RUN usermod -aG audio vscode

# --- STAGE: Fuzzing Toolchain ---
# I am isolating the Clang 18 installation. 
# This confirms if Clang 18 is compatible with our Ubuntu 24.04 base.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    clang-18 \
    llvm-18 \
    libfuzzer-18-dev \
    && rm -rf /var/lib/apt/lists/*