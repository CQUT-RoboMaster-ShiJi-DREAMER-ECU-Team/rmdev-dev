cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_peripheral_model CXX)

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)

    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core
        PRIVATE emdevif_compile_options
    )

    target_include_directories(${PROJECT_NAME} PUBLIC inc)

    file(GLOB emdevifPeripheralModelModules "modules/*.cppm")
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET emdevif_peripheral_models_modules TYPE CXX_MODULES FILES
            ${emdevifPeripheralModelModules}
    )
else ()
    add_library(${PROJECT_NAME} INTERFACE)
    target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_core
        emdevif_compile_options
    )
    target_include_directories(${PROJECT_NAME} INTERFACE inc)
endif ()
