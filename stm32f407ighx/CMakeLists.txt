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
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    rmdev
    stm32_hal_test_func_impl
)

if (RMDEV_ENABLE_TESTS)
    enable_testing()
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE rmdev_unit_test emdevif_test_framework)
endif ()

# For CMSIS-DSP

add_library(rmdev_cmsis_dsp_support INTERFACE)
target_link_directories(rmdev_cmsis_dsp_support INTERFACE ./Middlewares/ST/ARM/DSP/Lib/)
target_link_libraries(rmdev_cmsis_dsp_support INTERFACE :libarm_cortexM4lf_math.a stm32cubemx)

# For FreeRTOS-Kernel

add_library(freertos_config INTERFACE)

target_sources(freertos_config INTERFACE
    FILE_SET header_file TYPE HEADERS FILES ./FreeRTOSConfig.h
)

target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)
set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)

add_library(emdevif_from_user_impl OBJECT)
target_sources(emdevif_from_user_impl
    PUBLIC FILE_SET saefiu7gsuy7a TYPE CXX_MODULES FILES emdevif_device_handle_map_impl.cppm)
target_link_libraries(emdevif_from_user_impl PUBLIC emdevif_stm32cubemx_support emdevif_container)
