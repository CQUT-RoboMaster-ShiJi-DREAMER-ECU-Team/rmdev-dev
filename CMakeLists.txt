cmake_minimum_required(VERSION 3.28)

project(rmdev_developing_or_testing_environment C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(RMDEV_ENABLE_TESTS "Enable test for rmdev" ON)
set(PLATFORM_NAME "" CACHE STRING "Set to choose run test on which platform.")
set(BOARD_NAME "" CACHE STRING "Set to choose run test on which board.")

if (RMDEV_ENABLE_TESTS)
    enable_testing()
endif ()

add_library(printf STATIC)
target_sources(printf PRIVATE libprintf/printf.c
    PUBLIC FILE_SET header TYPE HEADERS BASE_DIRS libprintf/ FILES libprintf/printf.h
)
target_compile_definitions(printf PRIVATE PRINTF_DISABLE_SUPPORT_EXPONENTIAL)

block()
    set(boardDir "")

    if (PLATFORM_NAME MATCHES "stm32")
        set(boardDir Platforms/STM32/)
    else ()
        message(FATAL_ERROR "Unknown platform name!")
    endif ()

    add_subdirectory(${boardDir}/${BOARD_NAME})

    add_subdirectory(rmdev)

    if (NOT RMDEV_FREERTOS_USE_STM32CUBEMX)
        add_subdirectory(FreeRTOS-Kernel)
    endif ()
endblock()
