
add_library(emdevif_compile_options INTERFACE)

set(EMDEVIF_MSVC_COMPILE_OPTIONS "/W4;/source-charset:utf-8" CACHE STRING
    "Set to config compile options if this project use MSVC compiler. Default value is \
    \"/W4;/source-charset:utf-8\"."
)
set(EMDEVIF_GNU_COMPILE_OPTIONS  "-Wall;-Wextra;-Wpedantic" CACHE STRING
    "Set to config compile options if this project use GNU compiler. Default value is \
    \"-Wall;-Wextra;-Wpedantic\"."
)

if (MSVC)
    target_compile_options(emdevif_compile_options INTERFACE
        ${EMDEVIF_MSVC_COMPILE_OPTIONS}
    )
else ()
    target_compile_options(emdevif_compile_options INTERFACE
        ${EMDEVIF_GNU_COMPILE_OPTIONS}
    )
endif ()
