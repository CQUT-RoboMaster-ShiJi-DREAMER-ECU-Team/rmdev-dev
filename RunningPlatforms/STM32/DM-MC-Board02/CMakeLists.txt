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
set(CMAKE_PROJECT_NAME DM-MC-Board02)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

add_subdirectory(Demo)

set(ENABLE_AFTER_UNIT_TEST_DEMO ON CACHE INTERNAL "" FORCE)

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
CPMAddPackage(
    NAME CMSIS-DSP
    GITHUB_REPOSITORY ARM-software/CMSIS-DSP
    GIT_TAG v1.17.0
    VERSION 1.17.0
    OPTIONS "CMSISCORE ${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS" "DISABLEFLOAT16 ON"
)
if (NOT CMSIS-DSP_ADDED)
    message(FATAL_ERROR "[${PROJECT_NAME}]: CPM: Failed to add package `CMSIS-DSP`")
endif ()
target_compile_options(CMSISDSP PRIVATE -ffast-math -fno-finite-math-only -Ofast)

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

set(RMDEV_ENABLED_DRIVER_LIST "BMI088;DJIMotor" CACHE STRING "" FORCE)
set(RMDEV_ENABLE_INS_MODULE ON CACHE BOOL "" FORCE)

add_library(emdevif_user_declares STATIC)
target_sources(emdevif_user_declares
    PUBLIC FILE_SET saefiu7gsuy7a TYPE CXX_MODULES FILES emdevif_user_declares.cppm)
target_link_libraries(emdevif_user_declares PUBLIC
    emdevif_stm32cubemx_support emdevif_core printf emdevif_system
    emdevif_stm32_peripheral
    PRIVATE emdevif_compile_options emdevif_peripheral_model
)
target_compile_definitions(emdevif_user_declares PUBLIC EMDEVIF_USER_DECLARES_PROVIDE_MODULE=1)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

set(EMDEVIF_USE_STM32CUBEMX ON)
set(EMDEVIF_ENABLE_TESTS ON)
set(EMDEVIF_FREERTOS_USE_STM32CUBEMX OFF)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif
    ${CMAKE_BINARY_DIR}/emdevif_collection/emdevif
)

set(RMDEV_ENABLE_TESTS ON)
add_subdirectory(${CMAKE_SOURCE_DIR}/rmdev ${CMAKE_BINARY_DIR}/rmdev)

target_compile_definitions(emdevif_system PUBLIC EMDEVIF_SYS_TICK_TYPE_UINT_BITS=32)

add_subdirectory(${CMAKE_SOURCE_DIR}/TestImplement ${CMAKE_BINARY_DIR}/TestImplement)

addCopyFinallyBinaryFileTarget(${PROJECT_NAME} "${PROJECT_NAME}.elf" ${CMAKE_CURRENT_BINARY_DIR})
