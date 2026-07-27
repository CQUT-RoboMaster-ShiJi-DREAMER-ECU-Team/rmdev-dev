cmake_minimum_required(VERSION 3.28...4.0.2)

project(rmdev_device_model CXX)

if (NOT (TARGET mp-units))
    find_package(mp-units REQUIRED)
endif ()

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)

    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core mp-units::mp-units
        PRIVATE emdevif_compile_options
    )

    target_include_directories(${PROJECT_NAME} PUBLIC inc)
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET ${PROJECT_NAME}_modules TYPE CXX_MODULES FILES
            sensor/Imu.cppm

            motor/Motor.cppm
    )
else ()
    add_library(${PROJECT_NAME} INTERFACE)
    target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_core emdevif_compile_options
        mp-units::mp-units
    )
    target_include_directories(${PROJECT_NAME} INTERFACE inc)
endif ()
