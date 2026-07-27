cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_peripheral_impl_ CXX)

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)

    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core emdevif_peripheral_handle_map_ emdevif_peripheral_model
        PRIVATE emdevif_compile_options
    )

    target_include_directories(${PROJECT_NAME} PUBLIC inc)
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET ${PROJECT_NAME}_modules TYPE CXX_MODULES FILES
        Gpio.cppm Pwm.cppm Serial.cppm Spi.cppm Timer.cppm Can.cppm
    )
else ()
    add_library(${PROJECT_NAME} INTERFACE)
    target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_core
        emdevif_peripheral_handle_map_ emdevif_peripheral_model
        emdevif_compile_options
    )
    target_include_directories(${PROJECT_NAME} INTERFACE inc)
endif ()
