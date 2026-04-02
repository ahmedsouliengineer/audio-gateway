# Audio Gateway

A high-performance C++23 gateway for real-time audio capture (ALSA) and network distribution (ZMQ).

🚀 Quick Start (Development)

I am choosing to leverage the DevContainer as the primary way to interact with this repo to ensure the C++23 toolchain is perfectly configured.

    Open in VS Code: Press F1 and select Dev Containers: Reopen in Container.

🛠 Prerequisites

    Docker & VS Code (Recommended)

    Host Audio: Ensure the host user has access to /dev/snd.

    Toolchain: Clang 18+ and libstdc++ 13 (if building outside Docker).

🏗 Architecture & Standards

I am choosing to maintain a strict engineering manifesto for this project. For detailed information on design decisions and infrastructure, see the Documentation folder:

    Engineering Standards: Rationale for C++23 choices, Clang-Tidy rules, and CI pipeline design.

    Hardware Setup: [Planned] Instructions for ALSA PCM configuration.

🧪 Quality Gates

This project enforces a Zero-Warning Policy. The following checks run on every pull request:

    Static Analysis: cppcheck & clang-tidy-18

    Testing: GTest with llvm-cov reporting.

    Fuzzing: LLVM LibFuzzer for robust buffer handling.

📦 Deployment

I am choosing a Multi-Stage Docker approach. To build a production-ready, minimal footprint image:
Bash

docker build --target release -t audio-gateway:latest .