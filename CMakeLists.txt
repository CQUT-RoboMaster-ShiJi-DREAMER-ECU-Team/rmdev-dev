cmake_minimum_required(VERSION 3.28)

project(rmdev_developing_or_testing_environment C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

if (NOT (DEFINED PLATFORM_NAME))
    message(WARNING "Unknown platform. Please set PLATFORM_NAME to choose witch platform to build.")
else ()
    add_subdirectory(rmdev)
    add_subdirectory(${PLATFORM_NAME})
endif ()
