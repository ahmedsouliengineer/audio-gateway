cmake_minimum_required(VERSION 3.24)
project(AudioGateway VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 1. FIND PACKAGES ---
find_package(ALSA REQUIRED)
find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)

# I am adding Boost.System to verify the link environment.
find_package(Boost REQUIRED COMPONENTS system)

# I am using PkgConfig to locate ZeroMQ.
pkg_check_modules(ZMQ REQUIRED IMPORTED_TARGET libzmq)

# --- 2. LIBRARY TARGET ---
add_library(gateway_lib STATIC)
target_include_directories(gateway_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# I am linking ALSA, ZeroMQ, and now Boost.
target_link_libraries(gateway_lib 
    PRIVATE 
        ALSA::ALSA 
        PkgConfig::ZMQ
        Boost::system
)

# I am including src/command_listener.cpp since we previously 
# implemented the ZeroMQ logic there.
target_sources(gateway_lib PRIVATE 
    src/alsa_impl.cpp 
    src/command_listener.cpp
)

# --- 3. EXECUTABLE TARGET ---
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE gateway_lib)

# --- 4. GOOGLETEST ---
enable_testing()
add_executable(unit_tests tests/unit/test_hardware.cpp)
target_link_libraries(unit_tests PRIVATE gateway_lib GTest::gtest GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(unit_tests)

# --- 5. FUZZING ---
option(ENABLE_FUZZING "Build the Fuzzer targets" OFF)
if(ENABLE_FUZZING AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_executable(fuzz_alsa tests/fuzz/fuzz_alsa.cpp)
    target_link_libraries(fuzz_alsa PRIVATE gateway_lib)
    target_compile_options(fuzz_alsa PRIVATE -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer)
    target_link_options(fuzz_alsa PRIVATE -fsanitize=fuzzer,address,undefined)
endif()