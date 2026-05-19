cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME RoboMasterDevelopmentBoardTypeC)

#include("cmake/gcc-arm-none-eabi.cmake") # 应当预定义 CMAKE_TOOLCHAIN_FILE 变量

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

add_subdirectory(Demo)

set(ENABLE_AFTER_UNIT_TEST_DEMO ON CACHE INTERNAL "" FORCE)
set(EMDEVIF_USE_CPP_MODULES OFF CACHE INTERNAL "" FORCE)

set(EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST "GPIO;USART;SPI;PWM" CACHE INTERNAL "" FORCE)
set(EMDEVIF_STM32_PERIPHERAL_DRIVER        "HAL;HAL;HAL;HAL" CACHE INTERNAL "" FORCE)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif_stm32_peripheral
    ${CMAKE_BINARY_DIR}/emdevif_collection/emdevif_stm32_peripheral
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    rmdev
    test_implement
)

if (RMDEV_ENABLE_TESTS)
    enable_testing()
endif ()

# For CMSIS-DSP
add_library(CMSISDSP INTERFACE)
target_link_directories(CMSISDSP INTERFACE ./Middlewares/ST/ARM/DSP/Lib/)
target_link_libraries(CMSISDSP INTERFACE :libarm_cortexM4lf_math.a stm32cubemx)

# For FreeRTOS-Kernel
add_library(freertos_config INTERFACE)
target_sources(freertos_config INTERFACE
    FILE_SET header_file TYPE HEADERS FILES ./FreeRTOSConfig.h
)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)
target_link_libraries(freertos_config INTERFACE emdevif_core)
CPMAddPackage(
    NAME FreeRTOS-Kernel
    GITHUB_REPOSITORY FreeRTOS/FreeRTOS-Kernel
    GIT_TAG V11.3.0
    VERSION 11.3.0
    OPTIONS "FREERTOS_HEAP 4" "FREERTOS_PORT GCC_ARM_CM4F"
)
if (NOT FreeRTOS-Kernel_ADDED)
    message(FATAL_ERROR "[${PROJECT_NAME}]: CPM: Failed to add package `FreeRTOS-Kernel`")
endif ()

set(RMDEV_ENABLED_DRIVER_LIST "BMI088" CACHE INTERNAL "" FORCE)
set(RMDEV_ENABLE_INS_MODULE ON CACHE INTERNAL "" FORCE)

add_library(emdevif_user_declares INTERFACE)
target_include_directories(emdevif_user_declares INTERFACE inc)
target_link_libraries(emdevif_user_declares INTERFACE
    emdevif_stm32cubemx_support emdevif_core printf emdevif_system
    emdevif_stm32_peripheral emdevif_compile_options emdevif_peripheral_model
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

set(EMDEVIF_USE_STM32CUBEMX ON)
set(EMDEVIF_ENABLE_TESTS ON)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif
    ${CMAKE_BINARY_DIR}/emdevif_collection/emdevif
)

set(RMDEV_ENABLE_TESTS ON)
set(RMDEV_ENABLED_DRIVER_LIST "DJIMotor" CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/rmdev ${CMAKE_BINARY_DIR}/rmdev)

add_subdirectory(${CMAKE_SOURCE_DIR}/TestImplement ${CMAKE_BINARY_DIR}/TestImplement)

addCopyFinallyBinaryFileTarget(${PROJECT_NAME} "${PROJECT_NAME}.elf" ${CMAKE_CURRENT_BINARY_DIR})
