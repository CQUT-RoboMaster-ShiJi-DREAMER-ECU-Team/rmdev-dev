cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_esp_idf_support C CXX)

add_library(${PROJECT_NAME} INTERFACE)

idf_component_get_property(emdevifEspIdfFreertosLibName freertos COMPONENT_ALIAS)

target_link_libraries(${PROJECT_NAME} INTERFACE
    ${emdevifEspIdfFreertosLibName}
)

target_compile_definitions(${PROJECT_NAME} INTERFACE
    EMDEVIF_THREAD_USE_ESPIDF_FREERTOS=1
)

message("[${PROJECT_NAME}]: usage ESP-IDF.")
