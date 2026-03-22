cmake_minimum_required(VERSION 3.24)

# Project metadata
project(AudioGateway VERSION 0.1.0 LANGUAGES CXX)

# Industrial C++23 standards
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for Clang-Tidy/Cppcheck
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Dependencies ---
find_package(ALSA REQUIRED)
find_package(GTest REQUIRED)

# --- Library Logic ---
# I am creating a static library to house our hardware implementation.
# This makes it easy to link against both the main app AND the tests.
add_library(gateway_lib STATIC src/alsa_impl.cpp)

# I am defining the 'include' directory as PUBLIC so that 
# any target linking to this library automatically sees your headers.
target_include_directories(gateway_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link ALSA to the library logic
target_link_libraries(gateway_lib PRIVATE ALSA::ALSA)

# --- Main Executable ---
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE gateway_lib)

# --- Unit Testing (GTest) ---
enable_testing()

# I am using GTest::gtest_main so we don't have to write a main() for tests.
add_executable(unit_tests tests/unit/test_hardware.cpp)

target_link_libraries(unit_tests 
    PRIVATE 
        gateway_lib 
        GTest::gtest 
        GTest::gtest_main
)

# Automated test discovery
include(GoogleTest)
gtest_discover_tests(unit_tests)

# --- Fuzz Testing (LibFuzzer) ---
# I renamed this to match your 'fuzz_device.cpp' file.
option(ENABLE_FUZZING "Build the Fuzzer targets" OFF)

if(ENABLE_FUZZING)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Updated source path to match your 'ls' output
        add_executable(fuzz_device tests/fuzz/fuzz_device.cpp)
        
        target_link_libraries(fuzz_device PRIVATE gateway_lib)
        
        # AddressSanitizer + UndefinedBehaviorSanitizer are vital for fuzzing
        target_compile_options(fuzz_device PRIVATE 
            -fsanitize=fuzzer,address,undefined)
        target_link_options(fuzz_device PRIVATE 
            -fsanitize=fuzzer,address,undefined)
    else()
        message(WARNING "Fuzzing skipped: Requires Clang. Current compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()








# cmake_minimum_required(VERSION 3.24)

# # I am naming the project. This name is used by the guard 
# # to ensure no other repo accidentally compiles this hardware driver.
# project(AudioGateway VERSION 1.0.0 LANGUAGES CXX)

# # I am enforcing C++23. I chose this specifically for std::expected 
# # and std::print, which we will use for our zero-mutex error handling.
# set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# # I am generating the compile_commands.json so that Clang-Tidy 
# # works perfectly inside our Ubuntu 24.04 Dev Container.
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# # --- 1. THE ARCHITECTURAL GUARD ---
# # I am preventing anyone from "stealing" this driver logic into 
# # another module. Only the AudioGateway is the hardware owner.
# if(NOT PROJECT_NAME STREQUAL "AudioGateway")
#     message(FATAL_ERROR "SECURITY ALERT: Hardware access denied for ${PROJECT_NAME}.")
# endif()

# # --- 2. EXTERNAL DEPENDENCIES ---
# # I am fetching our 'CoreUtils' repo. I chose FetchContent because 
# # it ensures every developer has the same version of the RingBuffer and Logger.
# include(FetchContent)
# FetchContent_Declare(
#     core_utils
#     GIT_REPOSITORY https://github.com/your-org/core-utils.git
#     GIT_TAG        main # In production, I would use a specific commit hash here.
# )
# FetchContent_MakeAvailable(core_utils)

# # I am finding the Linux ALSA headers.
# find_package(ALSA REQUIRED)

# # --- 3. THE PRIVATE HARDWARE LIBRARY ---
# # I am building the driver as a STATIC library. 
# # It is 'Private' to the world, but 'Public' to our internal tests.
# add_library(audio_hw_driver STATIC)

# # I am setting up the include paths.
# # I use BUILD_INTERFACE so our Unit Tests can see the internal headers,
# # but those headers won't be exported to any other repo.
# target_include_directories(audio_hw_driver 
#     PUBLIC 
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/internal>
#     INTERFACE
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/shared>
# )

# # I am linking the core utilities (IPC/Logging) and the hardware drivers.
# target_link_libraries(audio_hw_driver 
#     PRIVATE 
#         ALSA::ALSA
#         core_utils::ipc      # For the Mutex-free Ring Buffer
#         core_utils::logging  # For the C++23 std::print wrapper
# )

# target_sources(audio_hw_driver PRIVATE 
#     src/internal/alsa_impl.cpp
#     src/internal/audio_processor.cpp
# )

# # --- 4. THE SERVICE EXECUTABLE ---
# # I am creating the actual process that will run in the container.
# add_executable(${PROJECT_NAME} src/main.cpp)
# target_link_libraries(${PROJECT_NAME} PRIVATE audio_hw_driver)

# # --- 5. THE TEST GATEWAY ---
# # I am creating a test runner. Because it's in this same file, 
# # it can link to the 'Private' hardware library for unit testing.
# enable_testing()
# add_executable(unit_tests tests/test_main.cpp)
# target_link_libraries(unit_tests PRIVATE audio_hw_driver GTest::gtest_main)
# add_test(NAME AllTests COMMAND unit_tests)