
include(${CMAKE_CURRENT_LIST_DIR}/get_cpm.cmake)

if(DEFINED ENV{CPM_SOURCE_CACHE})
    message(STATUS "Found CPM_SOURCE_CACHE in environment, value=$ENV{CPM_SOURCE_CACHE}")
elseif(CPM_SOURCE_CACHE)
    message(STATUS "Defined CPM_SOURCE_CACHE as cmake variable, value=${CPM_SOURCE_CACHE}")
else()
    message(STATUS "Did not find CPM_SOURCE_CACHE, use default configure")
endif()

# 该工程不应该使用包锁文件，因为该仓库是一系列库的聚合，其子模块有不同的依赖关系，使用包锁文件会导致依赖冲突。
# CPMUsePackageLock(${CMAKE_CURRENT_LIST_DIR}/package-lock.cmake)

CPMAddPackage(
    NAME printf
    GITHUB_REPOSITORY mpaland/printf
    GIT_TAG v4.0.0
    VERSION 4.0.0
    DOWNLOAD_ONLY
)
if (printf_ADDED)
    add_library(printf STATIC)
    target_sources(printf
        PRIVATE ${printf_SOURCE_DIR}/printf.c

        PUBLIC FILE_SET header TYPE HEADERS
        BASE_DIRS ${printf_SOURCE_DIR}
        FILES ${printf_SOURCE_DIR}/printf.h
    )
else ()
    message(FATAL_ERROR "[${PROJECT_NAME}]: CPM: Failed to add package `printf`")
endif ()

CPMFindPackage(
    NAME mp-units
    GITHUB_REPOSITORY mpusz/mp-units
    GIT_TAG v2.5.0
    VERSION 2.5.0
    DOWNLOAD_ONLY ON
)
if (mp-units_ADDED)
    block (SCOPE_FOR POLICIES)
        cmake_policy(VERSION 3.28...4.0.2)

        set(MP_UNITS_BUILD_INSTALL OFF CACHE INTERNAL "" FORCE)
        set(MP_UNITS_BUILD_CXX_MODULES OFF CACHE INTERNAL "" FORCE)
        set(MP_UNITS_API_CONTRACTS "NONE" CACHE INTERNAL "" FORCE)
        set(MP_UNITS_API_FREESTANDING OFF CACHE INTERNAL "" FORCE)
        add_subdirectory("${mp-units_SOURCE_DIR}/src" "${mp-units_BINARY_DIR}")
    endblock ()
else ()
    message(FATAL_ERROR "[${PROJECT_NAME}]: CPM: Failed to add package `mp-units`")
endif ()

set(testRequiresCmsisDsp OFF)
if (TEST_RMDEV_MATH_IMPL STREQUAL "CMSIS-DSP")
    set(testRequiresCmsisDsp ON)
else ()
    message(FATAL_ERROR "[${PROJECT_NAME}]: Invalid value for TEST_RMDEV_MATH_IMPL: ${TEST_RMDEV_MATH_IMPL}")
endif ()

if (TEST_RMDEV_MATRIX_IMPL STREQUAL "CMSIS-DSP")
    set(testRequiresCmsisDsp ON)
elseif (TEST_RMDEV_MATRIX_IMPL STREQUAL "Eigen")
    CPMFindPackage(
        NAME Eigen3
        URL "https://gitlab.com/libeigen/eigen/-/archive/5.0.1/eigen-5.0.1.tar.gz"
        URL_HASH SHA256=E9C326DC8C05CD1E044C71F30F1B2E34A6161A3B6ECF445D56B53FF1669E3DEC
        VERSION 5.0.1
        OPTIONS "EIGEN_BUILD_TESTING OFF" "EIGEN_BUILD_DOC OFF"
    )
    if (NOT Eigen3_ADDED)
        message(FATAL_ERROR "[${PROJECT_NAME}]: CPM: Failed to add package `Eigen3`")
    endif ()
endif ()

if (testRequiresCmsisDsp)
    set(cmsisDspHost OFF)

    if (TEST_PLATFORM STREQUAL "mock")
        CPMFindPackage(
            NAME CMSIS_6
            GITHUB_REPOSITORY ARM-software/CMSIS_6
            GIT_TAG v6.3.0
            VERSION 6.3.0
            DOWNLOAD_ONLY ON
        )
        if (NOT CMSIS_6_ADDED)
            message(FATAL_ERROR "[${PROJECT_NAME}]: CPM: Failed to add package `CMSIS_6`")
        endif ()

        set(cmsisCoreDir "${CMSIS_6_SOURCE_DIR}/CMSIS/Core/Include")
        set(cmsisDspHost ON)
    endif ()

    CPMFindPackage(
        NAME CMSIS-DSP
        GITHUB_REPOSITORY ARM-software/CMSIS-DSP
        GIT_TAG v1.17.0
        VERSION 1.17.0
        OPTIONS "CMSISCORE ${cmsisCoreDir}" "DISABLEFLOAT16 ON" "HOST ${cmsisDspHost}"
    )
    if (NOT CMSIS-DSP_ADDED)
        message(FATAL_ERROR "[${PROJECT_NAME}]: CPM: Failed to add package `CMSIS-DSP`")
    endif ()
    target_compile_options(CMSISDSP PRIVATE -ffast-math -fno-finite-math-only -Ofast)

    unset(cmsisCoreDir)
    unset(cmsisDspHost)
endif ()
unset(testRequiresCmsisDsp)
