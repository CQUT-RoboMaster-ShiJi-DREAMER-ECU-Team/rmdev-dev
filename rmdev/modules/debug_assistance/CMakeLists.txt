cmake_minimum_required(VERSION 3.28...4.0.2)

project(rmdev_debug_assistance CXX)

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)

    target_include_directories(${PROJECT_NAME} PUBLIC inc)
    target_sources(${PROJECT_NAME} PUBLIC FILE_SET ${PROJECT_NAME}_modules TYPE CXX_MODULES
        FILES vofa.cppm
    )

    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core
        PRIVATE emdevif_compile_options
    )
else ()
    add_library(${PROJECT_NAME} INTERFACE)
    target_include_directories(${PROJECT_NAME} INTERFACE inc)
    target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_core emdevif_compile_options)
endif ()
