# --- STAGE 1: Development & Build Environment ---
FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04 AS developer

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libasound2-dev libgtest-dev libzmq3-dev \
    cppzmq-dev libboost-all-dev pkg-config \
    cmake g++ alsa-utils gdb cppcheck \
    clang-18 clang-tidy-18 clang-format-18 \
    llvm-18 libfuzzer-18-dev \
    && rm -rf /var/lib/apt/lists/*

RUN ln -s /usr/bin/clang-18 /usr/bin/clang && \
    ln -s /usr/bin/clang++-18 /usr/bin/clang++ && \
    ln -s /usr/bin/clang-tidy-18 /usr/bin/clang-tidy && \
    ln -s /usr/bin/run-clang-tidy-18 /usr/bin/run-clang-tidy && \
    ln -s /usr/bin/llvm-cov-18 /usr/bin/llvm-cov && \
    ln -s /usr/bin/llvm-profdata-18 /usr/bin/llvm-profdata

RUN cd /usr/src/googletest && cmake . && cmake --build . && cp lib/*.a /usr/lib

RUN usermod -aG audio vscode
WORKDIR /app

# --- STAGE 2: Tester (The Quality Gate) ---
FROM developer AS tester

# 1. Copy Configs (Rarely change - cached)
COPY CMakeLists.txt .clang-format .clang-tidy ./

# 2. Copy Source/Tests (Change often)
COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/

RUN cppcheck --enable=all --suppress=missingIncludeSystem src/ include/ --error-exitcode=1

RUN cmake -B build_cov -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DENABLE_COVERAGE=ON && \
    cmake --build build_cov && \
    cd build_cov && ctest --output-on-failure && \
    llvm-profdata merge -sparse default.profraw -o test.profdata && \
    llvm-cov report ./unit_tests -instr-profile=test.profdata ../src/

# --- STAGE 3: Builder (Production Compilation) ---
FROM developer AS builder

# We reuse the same specific copy strategy to ensure no local trash enters the binary
COPY CMakeLists.txt ./
COPY include/ ./include/
COPY src/ ./src/

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DENABLE_FUZZING=ON && \
    cmake --build build --target AudioGateway

# --- STAGE 4: Release (Minimal Runtime) ---
FROM ubuntu:24.04 AS release
RUN apt-get update && apt-get install -y --no-install-recommends \
    libasound2t64 libzmq5 libboost-system1.83.0 alsa-utils \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m gateway_service && usermod -aG audio gateway_service
USER gateway_service
WORKDIR /app

COPY --from=builder /app/build/AudioGateway .

ENTRYPOINT ["./AudioGateway"]