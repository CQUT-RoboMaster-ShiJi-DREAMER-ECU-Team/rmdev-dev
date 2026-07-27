cmake_minimum_required(VERSION 3.28...4.0.2)

project(rmdev_driver_DJIMotor CXX)

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)

    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core
        PRIVATE emdevif_compile_options rmdev_device_model emdevif_peripheral
    )

    target_include_directories(${PROJECT_NAME} PUBLIC inc)
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET ${PROJECT_NAME}_modules TYPE CXX_MODULES FILES
            dji_motor.cppm
    )
else ()
    add_library(${PROJECT_NAME} INTERFACE)
    target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_core emdevif_compile_options
        rmdev_device_model emdevif_peripheral
    )
    target_include_directories(${PROJECT_NAME} INTERFACE inc)
endif ()
