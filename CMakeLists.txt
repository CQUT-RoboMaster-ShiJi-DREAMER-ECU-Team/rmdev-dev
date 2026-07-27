cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif C CXX)

add_subdirectory(check_compile_features)

option(EMDEVIF_USE_CPP_MODULES "Enabled it to use C++ modules." ON)

if (EMDEVIF_USE_CPP_MODULES)
    if (NOT (EMDEVIF_COMPILER_SUPPORT_CPP20_MODULES))
        message(FATAL_ERROR "[${PROJECT_NAME}]: EMDEVIF_USE_CPP_MODULES is ON, "
            "but the compiler `${CMAKE_CXX_COMPILER}` does not support C++20 modules."
        )
    endif ()

    message(STATUS "[${PROJECT_NAME}]: Will use C++ modules.")
else ()
    message(STATUS "[${PROJECT_NAME}]: Will NOT use C++ modules.")
endif ()

set(emdevifAllModules "logger;timeline;system;peripheral")

set(EMDEVIF_ENABLED_MODULES ${emdevifAllModules} CACHE STRING
    "Set the modules to be enabled in emdevif, separated by semicolons(`;`). \
Default value is all modules(Listed in local variable `emdevifAllModules`)."
)

option(EMDEVIF_USE_STM32CUBEMX "Enable it will link stm32cubemx lib to emdevif" OFF)
option(EMDEVIF_USE_ESP_IDF "Enable it to use ESP-IDF." OFF)

include(cmake/utility.cmake)

emdevif_assertMutuallyExclusiveOptions(EMDEVIF_USE_STM32CUBEMX EMDEVIF_USE_ESP_IDF)

if (EMDEVIF_USE_STM32CUBEMX)
    enable_language(ASM)
    add_subdirectory(support/stm32cubemx)
elseif (EMDEVIF_USE_ESP_IDF)
    add_subdirectory(support/esp-idf)
endif ()

add_subdirectory(core)

add_library(${PROJECT_NAME} INTERFACE)

set(emdevifFinallyEnabledModules ${EMDEVIF_ENABLED_MODULES})

set(emdevifFinallyLinkedLibs "")
foreach (moduleName IN LISTS emdevifFinallyEnabledModules)
    add_subdirectory(${moduleName})

    list(APPEND emdevifFinallyLinkedLibs "emdevif_${moduleName}")

    message(STATUS "[${PROJECT_NAME}]: Enabled module: emdevif_${moduleName}")
endforeach ()

target_link_libraries(${PROJECT_NAME} INTERFACE
    emdevif_core
    ${emdevifFinallyLinkedLibs}
)
