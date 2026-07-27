cmake_minimum_required(VERSION 3.28...4.0.2)

project(rmdev_math CXX)

set(RMDEV_MATH_IMPL "CPP-STL" CACHE STRING
    "Implementation of rmdev_math module. \
Valid values are: CPP-STL, CMSIS-DSP"
)
set_property(CACHE RMDEV_MATH_IMPL PROPERTY STRINGS
    "CPP-STL;CMSIS-DSP"
)

set(RMDEV_MATH_MATRIX_IMPL "CMSIS-DSP" CACHE STRING
    "Implementation of rmdev_math matrix operations. \
Valid values are: CMSIS-DSP, Eigen"
)
set_property(CACHE RMDEV_MATH_MATRIX_IMPL PROPERTY STRINGS
    "CMSIS-DSP;Eigen"
)

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)

    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core
        PRIVATE emdevif_compile_options
    )

    target_include_directories(${PROJECT_NAME} PUBLIC inc)
    target_sources(${PROJECT_NAME} PUBLIC FILE_SET rmdev_math_modules
        TYPE CXX_MODULES FILES math.cppm Matrix.cppm
    )
else ()
    add_library(${PROJECT_NAME} INTERFACE)
    target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_core
        emdevif_compile_options
    )
    target_include_directories(${PROJECT_NAME} INTERFACE inc)
endif ()

message(DEBUG "[${PROJECT_NAME}]: RMDEV_MATH_IMPL: ${RMDEV_MATH_IMPL}")
message(DEBUG "[${PROJECT_NAME}]: RMDEV_MATH_MATRIX_IMPL: ${RMDEV_MATH_MATRIX_IMPL}")

if (RMDEV_MATH_IMPL STREQUAL "CPP-STL")
    # Nothing to do
elseif (RMDEV_MATH_IMPL STREQUAL "CMSIS-DSP")
    enable_language(C)

    if (EMDEVIF_USE_CPP_MODULES)
        target_include_directories(${PROJECT_NAME} PUBLIC impl/cmsis-dsp)
        target_link_libraries(${PROJECT_NAME} PUBLIC CMSISDSP)
        target_compile_definitions(${PROJECT_NAME} PUBLIC RMDEV_MATH_USE_CMSIS_DSP=1)
    else ()
        target_include_directories(${PROJECT_NAME} INTERFACE impl/cmsis-dsp)
        target_link_libraries(${PROJECT_NAME} INTERFACE CMSISDSP)
        target_compile_definitions(${PROJECT_NAME} INTERFACE RMDEV_MATH_USE_CMSIS_DSP=1)
    endif ()
else ()
    message(FATAL_ERROR "Invalid value for RMDEV_MATH_IMPL: ${RMDEV_MATH_IMPL}")
endif ()

if (RMDEV_MATH_MATRIX_IMPL STREQUAL "CMSIS-DSP")
    enable_language(C)

    if (EMDEVIF_USE_CPP_MODULES)
        target_include_directories(${PROJECT_NAME} PUBLIC impl/cmsis-dsp)
        target_link_libraries(${PROJECT_NAME} PUBLIC CMSISDSP)
        target_compile_definitions(${PROJECT_NAME} PUBLIC RMDEV_MATRIX_USE_CMSIS_DSP=1)
    else ()
        target_include_directories(${PROJECT_NAME} INTERFACE impl/cmsis-dsp)
        target_link_libraries(${PROJECT_NAME} INTERFACE CMSISDSP)
        target_compile_definitions(${PROJECT_NAME} INTERFACE RMDEV_MATRIX_USE_CMSIS_DSP=1)
    endif ()
elseif (RMDEV_MATH_MATRIX_IMPL STREQUAL "Eigen")
    if (NOT TARGET Eigen3::Eigen)
        find_package(Eigen3 REQUIRED)
    endif ()

    if (EMDEVIF_USE_CPP_MODULES)
        target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
        target_include_directories(${PROJECT_NAME} PUBLIC impl/eigen)
        target_compile_definitions(${PROJECT_NAME} PUBLIC RMDEV_MATRIX_USE_EIGEN=1)
    else ()
        target_link_libraries(${PROJECT_NAME} INTERFACE Eigen3::Eigen)
        target_include_directories(${PROJECT_NAME} INTERFACE impl/eigen)
        target_compile_definitions(${PROJECT_NAME} INTERFACE RMDEV_MATRIX_USE_EIGEN=1)
    endif ()
else ()
    message(FATAL_ERROR "Invalid value for RMDEV_MATH_MATRIX_IMPL: ${RMDEV_MATH_MATRIX_IMPL}")
endif ()
