cmake_minimum_required(VERSION 3.28...4.2.0)

set(EMDEVIF_TEST_TEST_SOURCE_DIR_
    "${CMAKE_CURRENT_LIST_DIR}/emdevif_collection/emdevif/test"
    CACHE INTERNAL
    [[Path to emdevif test source directory. Used for cmake scripts to include the scripts in this directory.
It's an internal variable, DO NOT MODIFY!]]
    FORCE
)

if (NOT (PLATFORM_NAME STREQUAL "esp32"))
    project(rmdev-dev C CXX ASM)
endif ()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_SCAN_FOR_MODULES "${EMDEVIF_USE_CPP_MODULES}" CACHE INTERNAL "" FORCE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(RMDEV_ENABLE_TESTS "Enable test for rmdev" ON)
set(PLATFORM_NAME "" CACHE STRING "Set to choose run test on which platform.")
set(BOARD_NAME "" CACHE STRING "Set to choose run test on which board.")

message(STATUS "CMAKE_CXX_COMPILER_ID=\"${CMAKE_CXX_COMPILER_ID}\"")
if (NOT MSVC)
    message(STATUS "Adding compile option: -fconcepts-diagnostics-depth=8")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fconcepts-diagnostics-depth=8>)
endif ()

if (RMDEV_ENABLE_TESTS)
    enable_testing()
    set(EMDEVIF_ENABLE_TESTS ON CACHE BOOL "" FORCE)
    set(RMDEV_ENABLE_TESTS ON CACHE BOOL "" FORCE)
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(utils)
include(add_packages)

set(boardDir "${CMAKE_CURRENT_LIST_DIR}")

if (PLATFORM_NAME STREQUAL "stm32")
    set(boardDir ${boardDir}/RunningPlatforms/STM32)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_definitions(USE_FULL_ASSERT=1)
    endif ()
elseif (PLATFORM_NAME STREQUAL "esp32")
    set(boardDir ${boardDir}/RunningPlatforms/ESP32)

    if (NOT (EXISTS ${boardDir}/${BOARD_NAME}))
        message(FATAL_ERROR "Directory `${boardDir}/${BOARD_NAME}\' does not exist! \
                This may be caused by an incorrect BOARD_NAME or this board have not been supported yet.")
    endif ()
    list(APPEND EXTRA_COMPONENT_DIRS "${boardDir}/${BOARD_NAME}/main")

    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    set(SDKCONFIG_DEFAULTS "${boardDir}/${BOARD_NAME}/sdkconfig.defaults")
    project(rmdev-dev)

    message(STATUS "In CMAKE_SOURCE_DIR >>>> EXTRA_COMPONENT_DIRS: ${EXTRA_COMPONENT_DIRS}")

    return()
else ()
    message(FATAL_ERROR "Unknown platform named `${PLATFORM_NAME}\'. "
        "Currently only \"stm32\" and \"esp32\" are supported."
    )
endif ()

if (NOT (EXISTS ${boardDir}/${BOARD_NAME}))
    message(FATAL_ERROR "Directory `${boardDir}/${BOARD_NAME}\' does not exist! "
        "This may be caused by an incorrect BOARD_NAME or this board have not been supported yet."
    )
endif ()
add_subdirectory(${boardDir}/${BOARD_NAME})
message(STATUS "In CMAKE_SOURCE_DIR >>>> Board dir: ${boardDir}/${BOARD_NAME}")
