cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_timeline CXX)

set(EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS OFF CACHE BOOL
    "Timeline time source is millisecond precision (ON) or microsecond precision (OFF)"
)

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)

    target_include_directories(${PROJECT_NAME} PUBLIC inc)

    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET ${PROJECT_NAME}_modules TYPE CXX_MODULES FILES
            timeline.cppm
    )

    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core
        PRIVATE emdevif_compile_options
    )
else ()
    add_library(${PROJECT_NAME} INTERFACE)

    target_include_directories(${PROJECT_NAME} INTERFACE inc)
    target_link_libraries(${PROJECT_NAME} INTERFACE
        emdevif_core emdevif_compile_options
    )
endif ()

if (EMDEVIF_USE_CPP_MODULES)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS=$<BOOL:${EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS}>
    )
else ()
    target_compile_definitions(${PROJECT_NAME} INTERFACE
        EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS=$<BOOL:${EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS}>
    )
endif ()
