cmake_minimum_required(VERSION 3.24)
project(AudioGateway VERSION 0.1.0 LANGUAGES CXX)

# I am sticking with C++23 for std::expected and std::print.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 1. FIND PACKAGES ---
find_package(ALSA REQUIRED)
find_package(GTest REQUIRED)

# I am switching back to the standard Module mode but removing the 'CONFIG' 
# requirement which was failing in CI. This lets CMake use its internal 
# FindBoost.cmake script to locate the Ubuntu system libraries.
find_package(Boost REQUIRED COMPONENTS system thread)

# --- 2. ZEROMQ DETECTION (PkgConfig) ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED IMPORTED_TARGET libzmq)

# --- 3. LIBRARY TARGET ---
add_library(gateway_lib STATIC)
target_include_directories(gateway_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# I am linking the hardware and network dependencies.
# I am using Boost_LIBRARIES and Boost_INCLUDE_DIRS which are the 
# standard variables populated by the FindBoost module.
target_link_libraries(gateway_lib 
    PRIVATE 
        ALSA::ALSA 
        PkgConfig::ZMQ
        Boost::system
        Boost::thread
)
target_sources(gateway_lib PRIVATE src/alsa_impl.cpp)

# --- 4. EXECUTABLE TARGET ---
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE gateway_lib)

# --- 5. GOOGLETEST UNIT TESTING ---
enable_testing()
add_executable(unit_tests tests/test_hardware.cpp)
target_link_libraries(unit_tests PRIVATE gateway_lib GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)

# --- 6. FUZZ TESTING (LibFuzzer) ---
option(ENABLE_FUZZING "Build the Fuzzer targets" OFF)

if(ENABLE_FUZZING)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_executable(fuzz_alsa tests/fuzz_alsa.cpp)
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