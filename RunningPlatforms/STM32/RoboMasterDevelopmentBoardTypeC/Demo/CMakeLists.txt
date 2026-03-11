cmake_minimum_required(VERSION 3.28...4.0.2)

project(AfterUnitTestDemo CXX)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PRIVATE demo_entry.cpp INS_task.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_link_libraries(${PROJECT_NAME}
    PRIVATE emdevif_core emdevif_compile_options rmdev emdevif_stm32_peripheral
)
