cmake_minimum_required(VERSION 3.24)
project(AudioGateway VERSION 0.1.0 LANGUAGES CXX)

# I am sticking with C++23 for std::expected and std::print.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 1. FIND PACKAGES ---
# I am requiring ALSA for audio, ZeroMQ for networking, and Boost for concurrency.
find_package(ALSA REQUIRED)
find_package(GTest REQUIRED)
find_package(ZeroMQ REQUIRED)
# I am selecting Boost System and Thread for the core engine logic.
find_package(Boost REQUIRED COMPONENTS system thread)

# --- 2. LIBRARY TARGET ---
add_library(gateway_lib STATIC)
target_include_directories(gateway_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# I am linking the hardware and network dependencies here.
target_link_libraries(gateway_lib 
    PRIVATE 
        ALSA::ALSA 
        libzmq 
        Boost::system 
        Boost::thread
)
target_sources(gateway_lib PRIVATE src/alsa_impl.cpp)

# --- 3. EXECUTABLE TARGET ---
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE gateway_lib)

# --- 4. GOOGLETEST UNIT TESTING ---
enable_testing()
# I am targeting the specific test file in your tests directory.
add_executable(unit_tests tests/unit/test_hardware.cpp)
target_link_libraries(unit_tests PRIVATE gateway_lib GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)

# --- 5. FUZZ TESTING (LibFuzzer) ---
option(ENABLE_FUZZING "Build the Fuzzer targets" OFF)

if(ENABLE_FUZZING)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # I am using the specific path to your fuzzer target.
        add_executable(fuzz_alsa tests/fuzz/fuzz_alsa.cpp)
        target_link_libraries(fuzz_alsa PRIVATE gateway_lib)
        
        target_compile_options(fuzz_alsa PRIVATE 
            -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer)
        target_link_options(fuzz_alsa PRIVATE 
            -fsanitize=fuzzer,address,undefined)
            
        message(STATUS "Fuzzing Logic: Target 'fuzz_alsa' is configured with LLVM Sanitizers.")
    else()
        message(FATAL_ERROR "Fuzzing Error: Requires Clang compiler. Current: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()