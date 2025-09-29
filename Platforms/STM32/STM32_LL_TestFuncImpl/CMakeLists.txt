cmake_minimum_required(VERSION 3.28)

project(stm32_ll_test_func_impl C)

add_library(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME} PRIVATE  emdevif_stm32cubemx_support emdevif_test_framework)

target_sources(${PROJECT_NAME}
    PRIVATE stm32_ll_test_func_impl.c

    PUBLIC FILE_SET header_file TYPE HEADERS FILES stm32_ll_test_func_impl.h
)
