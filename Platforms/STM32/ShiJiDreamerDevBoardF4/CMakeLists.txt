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
set(CMAKE_PROJECT_NAME ShiJiDreamerDevBoardF4)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

add_subdirectory(cmake/stm32cubemx)

set(EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST "USART;PWM" CACHE STRING "" FORCE)
set(EMDEVIF_STM32_PERIPHERAL_DRIVER        "HAL;HAL" CACHE STRING "" FORCE)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/rmdev/emdevif_home/emdevif_stm32_peripheral
    ${CMAKE_BINARY_DIR}/rmdev/emdevif_home/emdevif_stm32_peripheral
)

add_library(emdevif_user_declares STATIC)
target_sources(emdevif_user_declares
    PUBLIC FILE_SET saefiu7gsuy7a TYPE CXX_MODULES FILES emdevif_user_declares.cppm)

add_subdirectory(Demo)

set(ENABLE_AFTER_UNIT_TEST_DEMO ON CACHE BOOL "" FORCE)
add_subdirectory(../STM32_HAL_TestFuncImpl ../STM32_HAL_TestFuncImpl)

target_link_libraries(emdevif_user_declares PUBLIC
    emdevif_stm32cubemx_support emdevif_core emdevif_container printf emdevif_stm32_peripheral
)

target_sources(printf PRIVATE FILE_SET config TYPE HEADERS FILES printf_config.h)

set(CMSISCORE "${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS" CACHE STRING "" FORCE)
set(DISABLEFLOAT16 ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/CMSIS-DSP ${CMAKE_BINARY_DIR}/CMSIS-DSP)
target_compile_options(CMSISDSP PRIVATE -ffast-math -fno-finite-math-only -Ofast)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    rmdev
    stm32_hal_test_func_impl
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
