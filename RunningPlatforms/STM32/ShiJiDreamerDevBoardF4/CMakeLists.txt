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
set(CMAKE_PROJECT_NAME ShiJiDreamerDevBoardF4)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

add_subdirectory(cmake/stm32cubemx)

target_link_libraries(FreeRTOS PUBLIC emdevif_core)

set(EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST "USART;PWM" CACHE INTERNAL "" FORCE)
set(EMDEVIF_STM32_PERIPHERAL_DRIVER        "HAL;HAL" CACHE INTERNAL "" FORCE)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif_stm32_peripheral
    ${CMAKE_BINARY_DIR}/emdevif_collection/emdevif_stm32_peripheral
)

add_subdirectory(Demo)

set(ENABLE_AFTER_UNIT_TEST_DEMO ON CACHE INTERNAL "" FORCE)

add_library(emdevif_user_declares STATIC)
target_sources(emdevif_user_declares
    PUBLIC FILE_SET saefiu7gsuy7a TYPE CXX_MODULES FILES emdevif_user_declares.cppm)
target_link_libraries(emdevif_user_declares PUBLIC emdevif_core
    PRIVATE emdevif_stm32cubemx_support printf emdevif_stm32_peripheral
    emdevif_peripheral_model emdevif_system
)
target_compile_definitions(emdevif_user_declares PUBLIC EMDEVIF_USER_DECLARES_PROVIDE_MODULE=1)

set(CMSISCORE "${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS" CACHE INTERNAL "" FORCE)
set(DISABLEFLOAT16 ON CACHE INTERNAL "" FORCE)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/Packages/CMSIS-DSP
    ${CMAKE_BINARY_DIR}/Packages/CMSIS-DSP
)
target_compile_options(CMSISDSP PRIVATE -ffast-math -fno-finite-math-only -Ofast)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    rmdev
    test_implement
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

set(EMDEVIF_USE_STM32CUBEMX ON)
set(EMDEVIF_FREERTOS_USE_STM32CUBEMX ON)
set(EMDEVIF_ENABLE_TESTS ON)
set(EMDEVIF_FREERTOS_USE_STM32CUBEMX ON)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif
    ${CMAKE_BINARY_DIR}/emdevif_collection/emdevif
)

target_compile_definitions(emdevif_logger PUBLIC
    EMDEVIF_LOGGER_CONFIG_FILE="${CMAKE_CURRENT_LIST_DIR}/emdevif_logger_config.hpp"
)

set(RMDEV_ENABLE_TESTS ON)
set(RMDEV_ENABLED_DRIVER_LIST "DJIMotor" CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/rmdev ${CMAKE_BINARY_DIR}/rmdev)

target_compile_definitions(emdevif_system PUBLIC EMDEVIF_SYS_TICK_TYPE_UINT_BITS=32)

add_subdirectory(${CMAKE_SOURCE_DIR}/TestImplement ${CMAKE_BINARY_DIR}/TestImplement)

addCopyFinallyBinaryFileTarget(${PROJECT_NAME} "${PROJECT_NAME}.elf" ${CMAKE_CURRENT_BINARY_DIR})
