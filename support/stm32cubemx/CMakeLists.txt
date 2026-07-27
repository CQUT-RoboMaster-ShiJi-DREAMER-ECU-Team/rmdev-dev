cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_stm32cubemx_support C CXX ASM)

add_library(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME}
    PUBLIC stm32cubemx
    PRIVATE STM32_Drivers
)

if (EMDEVIF_USE_STM32CUBEMX AND EMDEVIF_FREERTOS_USE_STM32CUBEMX)
    target_link_libraries(${PROJECT_NAME} PUBLIC FreeRTOS)
endif ()

message(STATUS "[${PROJECT_NAME}]: Use STM32CubeMX.")
