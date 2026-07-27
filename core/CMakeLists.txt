cmake_minimum_required(VERSION 3.28...4.0.2)

include(cmake/config_compile_options.cmake)

project(emdevif_core C CXX)

add_library(${PROJECT_NAME} STATIC)

if (EMDEVIF_USE_CPP_MODULES)
    target_compile_definitions(${PROJECT_NAME} PUBLIC EMDEVIF_USE_MODULES=1)

    set(BOOST_USE_MODULES ON CACHE INTERNAL
        "This value was set to ON with \"FORCE\"(Because of `EMDEVIF_USE_CPP_MODULES\' \
is ON), DO NOT EDIT IT!"
        FORCE
    )
else ()
    set(BOOST_USE_MODULES OFF CACHE INTERNAL
        "This value was set to OFF with \"FORCE\"(Because of `EMDEVIF_USE_CPP_MODULES\' \
is OFF), DO NOT EDIT IT!"
        FORCE
    )
endif ()

include(cmake/get_cpm.cmake)
CPMFindPackage(
    NAME boost_pfr
    GITHUB_REPOSITORY boostorg/pfr
    GIT_TAG "boost-1.91.0"
    VERSION 1.89.0
)
if (NOT boost_pfr_ADDED)
    message(FATAL_ERROR "[${PROJECT_NAME}]: CPM: Failed to add package `boost_pfr`")
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC Boost::pfr)

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
