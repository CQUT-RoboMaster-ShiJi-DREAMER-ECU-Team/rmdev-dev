cmake_minimum_required(VERSION 3.28...4.0.2)

project(AfterUnitTestDemo CXX)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PUBLIC FILE_SET dxzbsabef TYPE CXX_MODULES FILES
    demo_entry.cppm INS_task.cppm
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC emdevif_core
    PRIVATE emdevif_compile_options rmdev emdevif_stm32_peripheral
)
