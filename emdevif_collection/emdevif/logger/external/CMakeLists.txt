cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_logger_external CXX)

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)

    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core
        PRIVATE emdevif_compile_options
    )

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../inc)

    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET emdevif_logger_modules TYPE CXX_MODULES FILES logger_external.cppm
    )

    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
    target_compile_definitions(${PROJECT_NAME} PUBLIC EMDEVIF_LOGGER_MODE=2)
else ()
    add_library(${PROJECT_NAME} INTERFACE)
    target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_core emdevif_compile_options)
    target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
    target_compile_definitions(${PROJECT_NAME} INTERFACE EMDEVIF_LOGGER_MODE=2)
endif ()
