cmake_minimum_required(VERSION 3.31...4.2.0)

project(rmdev-dev C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_SCAN_FOR_MODULES "${EMDEVIF_USE_CPP_MODULES}")

message(DEBUG "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(DEBUG "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
if (NOT MSVC)
    message(DEBUG "Adding compile option: -fconcepts-diagnostics-depth=8")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fconcepts-diagnostics-depth=8>)
endif ()

set(TEST_PLATFORM "mock" CACHE STRING "The platform to test on(mock, stm32, esp32, etc.)")
option(TEST_ENABLE_EXCEPTIONS "Enable exceptions in tests" OFF)

set(TEST_RMDEV_MATH_IMPL "CMSIS-DSP" CACHE STRING "Implementation of rmdev_math module.")
set(TEST_RMDEV_MATRIX_IMPL "CMSIS-DSP" CACHE STRING
    "Implementation of rmdev_math matrix operations."
)
message(DEBUG "TEST_RMDEV_MATH_IMPL: ${TEST_RMDEV_MATH_IMPL}")
message(DEBUG "TEST_RMDEV_MATRIX_IMPL: ${TEST_RMDEV_MATRIX_IMPL}")

message(DEBUG "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
if ((NOT CMAKE_TOOLCHAIN_FILE) AND (NOT TEST_ENABLE_EXCEPTIONS))
    if (MSVC)
        set(cxxCompileOptions /EHs- /GR-)
    else ()
        set(cxxCompileOptions -fno-exceptions -fno-rtti)
    endif ()

    foreach (opt IN LISTS cxxCompileOptions)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${opt}>)
    endforeach ()

    unset(cxxCompileOptions)
endif ()

string(TOLOWER "${TEST_PLATFORM}" TEST_PLATFORM)

message(DEBUG "TEST_PLATFORM: ${TEST_PLATFORM}")
message(DEBUG "TEST_ENABLE_EXCEPTIONS: ${TEST_ENABLE_EXCEPTIONS}")

include(cmake/utils.cmake)
include(cmake/add_packages.cmake)

if (TEST_PLATFORM STREQUAL "mock")
    set(EMDEVIF_ENABLED_MODULES timeline peripheral)
    add_subdirectory(emdevif_collection/emdevif)

    add_subdirectory(rmdev/modules/rmdev_message_manager)
    add_subdirectory(rmdev/modules/rmdev_debug_assistance)

    set(RMDEV_MATH_IMPL ${TEST_RMDEV_MATH_IMPL})
    set(RMDEV_MATRIX_IMPL ${TEST_RMDEV_MATRIX_IMPL})
    add_subdirectory(rmdev/modules/rmdev_math)

    enable_testing()
    add_subdirectory(tests/mock)
else ()
    message(FATAL_ERROR "Unsupported TEST_PLATFORM: ${TEST_PLATFORM}")
endif ()
