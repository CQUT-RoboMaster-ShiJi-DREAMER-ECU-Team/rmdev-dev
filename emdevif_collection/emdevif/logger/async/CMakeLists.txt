cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_logger_async CXX)

if (NOT EMDEVIF_LOGGER_LINK_SYSTEM_LIB)
    message(FATAL_ERROR
        "[${PROJECT_NAME}]: EMDEVIF_LOGGER_LINK_SYSTEM_LIB must be ON for async logger mode."
    )
endif ()

add_library(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME}
    PUBLIC emdevif_core
    PRIVATE emdevif_compile_options emdevif_system
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../inc)

target_sources(${PROJECT_NAME} PRIVATE async.cpp)

if (EMDEVIF_USE_CPP_MODULES)
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET emdevif_logger_modules TYPE CXX_MODULES FILES logger_async.cppm
    )
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
endif ()

target_compile_definitions(${PROJECT_NAME} PUBLIC EMDEVIF_LOGGER_MODE=1)
