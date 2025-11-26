add_library(printf STATIC)
target_sources(printf PRIVATE Packages/mpaland-printf/printf.c
    PUBLIC FILE_SET header TYPE HEADERS BASE_DIRS Packages/mpaland-printf
        FILES Packages/mpaland-printf/printf.h
)
target_compile_definitions(printf PRIVATE PRINTF_DISABLE_SUPPORT_EXPONENTIAL)
