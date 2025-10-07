cmake_minimum_required(VERSION 3.28)

set(ENABLE_AFTER_UNIT_TEST_DEMO OFF CACHE BOOL "Enable it to link lib \"AfterUnitTestDemo\" \
to run demo program in this platform.")

project(stm32_hal_test_func_impl C)

add_library(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME} PRIVATE
    rmdev
    rmdev_unit_test
    emdevif_stm32cubemx_support
    emdevif_user_declares
    emdevif_test_framework
    emdevif_stm32_peripheral
)

if (ENABLE_AFTER_UNIT_TEST_DEMO)
    target_link_libraries(${PROJECT_NAME} PRIVATE AfterUnitTestDemo)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_AFTER_UNIT_TEST_DEMO=1)
    message("[${PROJECT_NAME}]: Enabled demo.")
endif ()

target_sources(${PROJECT_NAME}
    PRIVATE stm32_hal_test_func_impl.cpp

    PUBLIC FILE_SET header_file TYPE HEADERS FILES stm32_hal_test_func_impl.h
)
