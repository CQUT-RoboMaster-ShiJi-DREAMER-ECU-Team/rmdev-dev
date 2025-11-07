cmake_minimum_required(VERSION 3.28)

project(emdevif_user_declares C CXX)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET ${PROJECT_NAME}_modules TYPE CXX_MODULES FILES
        user_decls.cppm
)

idf_component_get_property(uartLibName esp_driver_uart COMPONENT_ALIAS)

target_link_libraries(${PROJECT_NAME}
    PUBLIC emdevif_core
    PRIVATE emdevif_compile_options emdevif_peripheral_model ${uartLibName}
)
