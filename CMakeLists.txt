cmake_minimum_required(VERSION 3.28)

project(rmdev_developing_or_testing_environment C CXX ASM)

if (NOT (DEFINED PLATFORM_NAME))
    message(WARNING "Unknown platform. Please set PLATFORM_NAME to choose witch platform to build.")
else ()
    add_subdirectory(rmdev)
    add_subdirectory(${PLATFORM_NAME})
endif ()
