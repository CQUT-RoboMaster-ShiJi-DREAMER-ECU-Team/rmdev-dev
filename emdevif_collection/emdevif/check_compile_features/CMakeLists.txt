cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_check_compile_features C CXX)

message(STATUS "[${PROJECT_NAME}]: Checking compile features for emdevif...")

function(emdevif_messageCompileFeature_ compilerFeatureName isCompilerFeatureSupported)
    if (NOT (${isCompilerFeatureSupported}))
        message(FATAL_ERROR "[${PROJECT_NAME}]: The compiler \"${CMAKE_CXX_COMPILER}\" does "
            "not support ${compilerFeatureName}."
        )
    endif ()
endfunction()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if (EMDEVIF_USE_CPP_MODULES)
    message(STATUS "[${PROJECT_NAME}]: Checking C++20 modules support...")
    try_compile(EMDEVIF_COMPILER_SUPPORT_CPP20_MODULES
        SOURCES ${CMAKE_CURRENT_LIST_DIR}/cpp_modules_check.cpp
        LOG_DESCRIPTION "emdevif check C++20 modules support"
    )
    emdevif_messageCompileFeature_("C++20 modules" "${EMDEVIF_COMPILER_SUPPORT_CPP20_MODULES}")
endif ()
