
include(${CMAKE_CURRENT_LIST_DIR}/get_cpm.cmake)

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
