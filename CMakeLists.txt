cmake_minimum_required(VERSION 3.28)

project(rmdev_developing_or_testing_environment C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (RMDEV_ENABLE_TESTS)
    enable_testing()
endif ()

if (NOT (DEFINED PLATFORM_NAME))
    message(WARNING "Unknown platform. Please set PLATFORM_NAME to choose witch platform to build.")
else ()
    if (NOT (DEFINED RMDEV_FREERTOS_USE_STM32CUBEMX))
        set(RMDEV_FREERTOS_USE_STM32CUBEMX ON)
    endif ()

    add_library(printf STATIC)
    target_sources(printf PRIVATE libprintf/printf.c
        PUBLIC FILE_SET header TYPE HEADERS BASE_DIRS libprintf/ FILES libprintf/printf.h
    )
    target_compile_definitions(printf PRIVATE PRINTF_INCLUDE_CONFIG_H)

    add_subdirectory(${PLATFORM_NAME})

    add_subdirectory(rmdev)

    set(EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST USART)
    set(EMDEVIF_STM32_PERIPHERAL_DRIVER HAL)
    add_subdirectory(rmdev/emdevif_home/emdevif_stm32_peripheral)
    add_subdirectory(STM32_HAL_TestFuncImpl)
    add_subdirectory(STM32_LL_TestFuncImpl)

    if (NOT RMDEV_FREERTOS_USE_STM32CUBEMX)
        add_subdirectory(FreeRTOS-Kernel)
    endif ()
endif ()
