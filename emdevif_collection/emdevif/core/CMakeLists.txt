cmake_minimum_required(VERSION 3.28...4.0.2)

include(cmake/config_compile_options.cmake)

project(emdevif_core C CXX)

add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_sources(${PROJECT_NAME} PRIVATE
    src/error_handler/error_handler.cpp
    src/error_handler/fatal_handler_extern_c.cpp
)

if (EMDEVIF_USE_CPP_MODULES)
    file(GLOB_RECURSE emdevifCoreModules "modules/*.cppm")
    target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET ${PROJECT_NAME}_modules TYPE CXX_MODULES FILES
            ${emdevifCoreModules}
    )
endif ()
