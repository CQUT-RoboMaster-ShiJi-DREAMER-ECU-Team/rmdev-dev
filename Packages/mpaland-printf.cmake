# 由于 mpaland-printf 没有使用 CMake 构建系统，因此在这里将它作为静态库添加进来

add_library(printf STATIC)
target_sources(printf
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/mpaland-printf/printf.c

    PUBLIC FILE_SET header TYPE HEADERS
        BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/mpaland-printf
        FILES ${CMAKE_CURRENT_LIST_DIR}/mpaland-printf/printf.h
)
target_compile_definitions(printf PRIVATE PRINTF_DISABLE_SUPPORT_EXPONENTIAL)
