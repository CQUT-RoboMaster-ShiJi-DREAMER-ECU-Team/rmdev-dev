cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(RMDEV_ENABLE_TESTS "Enable test for rmdev" ON)
set(PLATFORM_NAME "" CACHE STRING "Set to choose run test on which platform.")
set(BOARD_NAME "" CACHE STRING "Set to choose run test on which board.")

if (RMDEV_ENABLE_TESTS)
    enable_testing()
    set(EMDEVIF_ENABLE_TESTS ON CACHE BOOL "" FORCE)
    set(RMDEV_ENABLE_TESTS ON CACHE BOOL "" FORCE)
endif ()

add_library(printf STATIC)
target_sources(printf PRIVATE libprintf/printf.c
    PUBLIC FILE_SET header TYPE HEADERS BASE_DIRS libprintf/ FILES libprintf/printf.h
)
target_compile_definitions(printf PRIVATE PRINTF_DISABLE_SUPPORT_EXPONENTIAL)

# 用于新增一个构建目标，这个目标用于将生成的二进制文件拷贝到 bin 目录以便于调试工具指定路径
function(addCopyFinallyBinaryFileTarget finalBinaryFileProjectName finalBinaryFileName finalBinaryFileDir)
    add_custom_command(
        TARGET ${finalBinaryFileProjectName} POST_BUILD
        COMMENT "Copy the binary file from cmake process binary dir to project_root/bin dir."
        COMMAND ${CMAKE_COMMAND} -E
            copy "${finalBinaryFileDir}/${finalBinaryFileName}"
            "${CMAKE_SOURCE_DIR}/bin/${finalBinaryFileName}"
        VERBATIM
    )

    set_property(DIRECTORY PROPERTY ADDITIONAL_CLEAN_FILES
        "${CMAKE_SOURCE_DIR}/bin"
    )

    message("[${PROJECT_NAME}](function `addCopyFinallyBinaryFileTarget\'): \n\
        finalBinaryFileProjectName: ${finalBinaryFileProjectName}\n\
        finalBinaryFileName: ${finalBinaryFileName}\n\
        finalBinaryFileDir: ${finalBinaryFileDir}"
    )
endfunction()

set(boardDir "${CMAKE_CURRENT_LIST_DIR}")

foreach (_ _)
    if (PLATFORM_NAME MATCHES "stm32")
        set(boardDir ${boardDir}/Platforms/STM32)

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            add_compile_definitions(USE_FULL_ASSERT=1)
        endif ()
    elseif (PLATFORM_NAME MATCHES "esp32")
        set(boardDir ${boardDir}/Platforms/ESP32)

        if (NOT (EXISTS ${boardDir}/${BOARD_NAME}))
            message(FATAL_ERROR "Directory `${boardDir}/${BOARD_NAME}\' does not exist! \
                This may be caused by an incorrect BOARD_NAME or this board have not been supported yet.")
        endif ()
        list(APPEND EXTRA_COMPONENT_DIRS "${boardDir}/${BOARD_NAME}")
        list(APPEND EXTRA_COMPONENT_DIRS "${boardDir}/${BOARD_NAME}/main")

        include($ENV{IDF_PATH}/tools/cmake/project.cmake)
        idf_build_set_property(MINIMAL_BUILD ON)
        set(SDKCONFIG_DEFAULTS "${boardDir}/${BOARD_NAME}/sdkconfig.defaults")
        project(rmdev_developing_or_testing_environment)

        message("In CMAKE_SOURCE_DIR >>>> EXTRA_COMPONENT_DIRS: ${EXTRA_COMPONENT_DIRS}")

        break ()
    else ()
        message(FATAL_ERROR "Unknown platform name!")
    endif ()

    project(rmdev_developing_or_testing_environment C CXX ASM)

    if (NOT (EXISTS ${boardDir}/${BOARD_NAME}))
        message(FATAL_ERROR "Directory `${boardDir}/${BOARD_NAME}\' does not exist! \
            This may be caused by an incorrect BOARD_NAME or this board have not been supported yet.")
    endif ()
    add_subdirectory(${boardDir}/${BOARD_NAME})
    message("In CMAKE_SOURCE_DIR >>>> Board dir: ${boardDir}/${BOARD_NAME}")
endforeach ()
