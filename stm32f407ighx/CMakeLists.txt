cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME stm32f407ighx)

#include("cmake/gcc-arm-none-eabi.cmake") # 应当预定义 CMAKE_TOOLCHAIN_FILE 变量

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    RmdevTestImpl/rmdev_test_impl.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    RmdevTestImpl/
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    rmdev
)

target_compile_definitions(emdevif_util PUBLIC EMDEVIF_UTIL_ARRAY_TRAITS_AGGREGATE_ADD_RESERVE_MEMBER=1)

if (RMDEV_ENABLE_TESTS)
    enable_testing()
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE rmdev_unit_test emdevif_test_framework)
endif ()

add_library(rmdev_cmsis_dsp_support INTERFACE)
target_link_directories(rmdev_cmsis_dsp_support INTERFACE ./Middlewares/ST/ARM/DSP/Lib/)
target_link_libraries(rmdev_cmsis_dsp_support INTERFACE :libarm_cortexM4lf_math.a stm32cubemx)
