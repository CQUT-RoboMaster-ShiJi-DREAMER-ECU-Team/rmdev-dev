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
set(CMAKE_PROJECT_NAME F103C8T6)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

target_link_libraries(FreeRTOS PUBLIC emdevif_core)

# 还没有适配 LL 库，不需要链接 emdevif_stm32_peripheral
# set(EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST "USART")
# set(EMDEVIF_STM32_PERIPHERAL_DRIVER        "LL")
# add_subdirectory(
#     ${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif_stm32_peripheral
#     ${CMAKE_BINARY_DIR}/emdevif_collection/emdevif_stm32_peripheral
# )

add_library(emdevif_user_declares STATIC)
target_sources(emdevif_user_declares
    PUBLIC FILE_SET user_decls TYPE CXX_MODULES FILES emdevif_user_declares.cppm)
target_link_libraries(emdevif_user_declares PUBLIC emdevif_core
    PRIVATE emdevif_stm32cubemx_support printf
    emdevif_peripheral_model emdevif_system
)

set(EMDEVIF_USE_STM32CUBEMX ON)
set(EMDEVIF_FREERTOS_USE_STM32CUBEMX ON)
set(EMDEVIF_ENABLE_TESTS ON)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif
    ${CMAKE_BINARY_DIR}/emdevif_collection/emdevif
)

# For CMSIS-DSP
add_library(CMSISDSP INTERFACE)
target_link_directories(CMSISDSP INTERFACE Middlewares/ST/ARM/DSP/Lib)
target_link_libraries(CMSISDSP INTERFACE :libarm_cortexM3l_math.a stm32cubemx)

set(RMDEV_ENABLE_TESTS ON)
add_subdirectory(${CMAKE_SOURCE_DIR}/rmdev ${CMAKE_BINARY_DIR}/rmdev)

target_compile_definitions(emdevif_system PUBLIC EMDEVIF_SYS_TICK_TYPE_UINT_BITS=32)

add_subdirectory(${CMAKE_SOURCE_DIR}/TestImplement ${CMAKE_BINARY_DIR}/TestImplement)

addCopyFinallyBinaryFileTarget(${PROJECT_NAME} "${PROJECT_NAME}.elf" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    rmdev
    test_implement
)
