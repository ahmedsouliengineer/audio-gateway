cmake_minimum_required(VERSION 3.24)
project(AudioGateway VERSION 0.1.0 LANGUAGES CXX)

# I am sticking with C++23 for std::expected and std::print.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ALSA REQUIRED)
find_package(GTest REQUIRED)

add_library(gateway_lib STATIC)
target_include_directories(gateway_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(gateway_lib PRIVATE ALSA::ALSA)
target_sources(gateway_lib PRIVATE src/alsa_impl.cpp)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE gateway_lib)

# --- GOOGLETEST UNIT TESTING ---
enable_testing()
add_executable(unit_tests tests/unit/test_hardware.cpp)
target_link_libraries(unit_tests PRIVATE gateway_lib GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)

# --- 5. FUZZ TESTING (LibFuzzer) ---
# I am adding an option to toggle fuzzing. It is OFF by default to 
# avoid interfering with standard GCC builds.
option(ENABLE_FUZZING "Build the Fuzzer targets" OFF)

if(ENABLE_FUZZING)
    # I am forcing a check for Clang. libFuzzer is tightly integrated with 
    # the LLVM toolchain we installed in the Dockerfile.
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_executable(fuzz_alsa tests/fuzz/fuzz_alsa.cpp)
        target_link_libraries(fuzz_alsa PRIVATE gateway_lib)
        
        # I am enabling AddressSanitizer and UndefinedBehaviorSanitizer.
        # These are the "eyes" of the fuzzer that catch memory corruption.
        target_compile_options(fuzz_alsa PRIVATE 
            -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer)
        target_link_options(fuzz_alsa PRIVATE 
            -fsanitize=fuzzer,address,undefined)
            
        message(STATUS "Fuzzing Logic: Target 'fuzz_alsa' is configured with LLVM Sanitizers.")
    else()
        message(FATAL_ERROR "Fuzzing Error: Requires Clang compiler. Current: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()