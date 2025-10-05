cmake_minimum_required(VERSION 3.28)

project(stm32_hal_test_func_impl C)

add_library(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME} PRIVATE
    rmdev
    rmdev_unit_test
    emdevif_stm32cubemx_support
    emdevif_user_declares
    emdevif_test_framework
    emdevif_stm32_peripheral
    AfterUnitTestDemo
)

target_sources(${PROJECT_NAME}
    PRIVATE stm32_hal_test_func_impl.cpp

    PUBLIC FILE_SET header_file TYPE HEADERS FILES stm32_hal_test_func_impl.h
)
