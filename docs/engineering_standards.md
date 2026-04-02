This document serves as the Engineering Constitution for the Audio Gateway project. It details the rationale behind our architectural choices, the rigor of our CI pipeline, and the specific configurations that ensure our code remains safe, modern, and high-performance.

1. Project Architecture & Build System

I am choosing a structure that prioritizes separation of concerns and build predictability.
Repository Layout

    include/ vs src/: I am choosing to separate headers from implementation to enforce a clean public API. Only files intended for external consumption reside in include/.

    tests/: Dedicated sub-directories for unit/ and fuzz/ ensure that quality checks are first-class citizens of the codebase.

CMake & C++23 Philosophy

I am choosing C++23 as our base standard to utilize modern error handling (like std::expected) and improved template metaprogramming.

    Toolchain Hardening: I am choosing to manually override the concept macro and point to libstdc++ 13. This bypasses header guard conflicts when using Clang 18 with the GNU standard library, ensuring we have full access to C++23 features.

    Strict Compilation: CMAKE_CXX_EXTENSIONS is set to OFF to ensure our code remains standard-compliant and portable, avoiding reliance on compiler-specific "magic".

    The "libstdc++ 13" Compatibility Hack: I am choosing to manually override the concept macro and point to libstdc++ 13. This bypasses header guard conflicts when using Clang 18 on Ubuntu 24.04, ensuring full access to <expected> and <print>.

    Note: Changing the underlying OS or GCC version requires updating this hardcoded path in CMakeLists.txt.

2. Development Environment (Docker & DevContainer)

I am choosing a container-first development workflow to eliminate the "it works on my machine" syndrome.
Multi-Stage Dockerfile

Our Dockerfile is organized into four distinct stages to balance developer utility with production security:

    developer: A heavyweight stage containing the full LLVM 18 suite, ALSA headers, and debugging tools like gdb.

    tester: Acts as a Quality Gate. It copies the source, runs cppcheck, executes unit tests, and generates coverage reports. If this stage fails, the image cannot be built.

    builder: A clean stage that compiles the final production binary using Release optimizations and Clang++.

    release: Our final production image. I am choosing a minimal ubuntu:24.04 base, installing only the necessary runtime libraries (libasound2t64, libzmq5) to minimize the attack surface.

DevContainer & Hardware Access

    Privileged Mode: I am choosing to run the container with --privileged and map /dev/snd to allow the gateway to interact directly with host audio hardware.

    Clangd Integration: I am choosing clangd over Microsoft's IntelliSense to provide more accurate C++23 navigation and real-time clang-tidy feedback within the editor.

    User Permissions: I am explicitly adding the vscode user to the audio group to grant the necessary permissions for ALSA device access.

    Troubleshooting: If audio fails, verify the host's /dev/snd is accessible and that the container user is correctly mapped to the host's audio group ID.

3. The CI/CD Pipeline (The "Eight Pillars")

The GitHub Actions pipeline (pipeline.yml) is designed to be a mirrored validation of our Docker environment.
Automated Checks

    Static Analysis (Cppcheck): Catches "C-style" errors that compilers often miss, such as uninitialized variables or manual memory management slips.

    Linting (Clang-Tidy): Enforces our "Modern C++" manifesto.

    Coverage Analysis: I am choosing to use llvm-cov to ensure that our unit tests are actually exercising the code, not just passing.

    Fuzzing Smoke Test: I am choosing to run fuzz_alsa for 10 seconds on every push to detect crashes when the gateway receives malformed audio packets.

4. Code Style & Static Analysis Constants

I am choosing to treat all linter warnings as build-failing errors (WarningsAsErrors: '*').
Clang-Tidy Rationale

    modernize-use-std-print: I am choosing to leverage C++23's std::print patterns for more efficient and type-safe logging.

    readability-function-size: I am choosing a LineThreshold of 60 lines. This forces us to break down complex audio processing logic into small, testable, and readable functions.

    modernize-use-nullptr: I am strictly banning NULL or 0 for pointers to avoid ambiguity in overloaded functions.

Clang-Format Rationale

    BreakBeforeBraces: Custom: I am choosing to wrap braces after classes, functions, and namespaces to maximize vertical scanability while keeping control statements compact.

    ColumnLimit: 100: I am choosing a slightly wider limit than the traditional 80 to accommodate descriptive modern C++ type names and std::expected signatures without excessive wrapping.

5. Dependency Governance (Renovate)

I am choosing to automate our maintenance via Renovate Bot to ensure we never fall behind on security patches.

    Automerge: I am choosing to automatically merge minor and patch updates if the CI pipeline passes, reducing manual maintenance burden.

    Pinning: We strictly pin GitHub Action digests to prevent "supply chain attacks" where a compromised action could inject malicious code into our builds.