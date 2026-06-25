cmake_minimum_required(VERSION 3.28...4.3.3)

project(emdevif_core_error_handler_test LANGUAGES CXX)

add_executable(${PROJECT_NAME} error_handler_test.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE emdevif_core)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
set_tests_properties(${PROJECT_NAME} PROPERTIES TIMEOUT 10)
