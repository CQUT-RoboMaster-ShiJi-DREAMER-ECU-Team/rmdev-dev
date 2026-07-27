cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_system C CXX)

set(EMDEVIF_THREAD_SYSTEM "FreeRTOS"
    CACHE STRING
    "Select witch system to use. Default value is \"FreeRTOS\""
)

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)

    target_include_directories(${PROJECT_NAME} PUBLIC inc)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core
        PRIVATE emdevif_compile_options
    )
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET emdevif_thread_modules TYPE CXX_MODULES FILES
            atomic.cppm sys_message_slot.cppm
    )
else ()
    add_library(${PROJECT_NAME} INTERFACE)
    target_include_directories(${PROJECT_NAME} INTERFACE inc)
    target_link_libraries(${PROJECT_NAME}
        INTERFACE emdevif_core emdevif_compile_options
    )
endif ()

if (EMDEVIF_USE_ESP_IDF)
    if (NOT (EMDEVIF_THREAD_SYSTEM STREQUAL "FreeRTOS"))
        message(WARNING "[${PROJECT_NAME}]: Usage ESP-IDF, so the value EMDEVIF_THREAD_SYSTEM\
        (was set to `${EMDEVIF_THREAD_SYSTEM}\') ignored and forced to be set with `FreeRTOS\'.")

        set(EMDEVIF_THREAD_SYSTEM "FreeRTOS")
    endif ()
endif ()

if (EMDEVIF_THREAD_SYSTEM STREQUAL "FreeRTOS")
    option(EMDEVIF_FREERTOS_USE_STM32CUBEMX "FreeRTOS will use stm32cubemx generated FreeRTOS sources" OFF)

    if (EMDEVIF_USE_ESP_IDF)
        if (EMDEVIF_FREERTOS_USE_STM32CUBEMX)
            message(WARNING "[${PROJECT_NAME}]: EMDEVIF_FREERTOS_USE_STM32CUBEMX is set to ON while usage \
            ESP-IDF(EMDEVIF_USE_ESP_IDF is ON), ignored.")
        endif ()

        if (EMDEVIF_USE_CPP_MODULES)
            target_link_libraries(${PROJECT_NAME} PUBLIC emdevif_esp_idf_support)
        else ()
            target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_esp_idf_support)
        endif ()
    elseif (EMDEVIF_FREERTOS_USE_STM32CUBEMX)
        if (NOT EMDEVIF_USE_STM32CUBEMX)
            message(FATAL_ERROR "Let FreeRTOS to use stm32cubemx requires to set EMDEVIF_USE_STM32CUBEMX equal to ON")
        endif ()

        if (EMDEVIF_USE_CPP_MODULES)
            target_link_libraries(${PROJECT_NAME} PUBLIC emdevif_stm32cubemx_support)
        else ()
            target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_stm32cubemx_support)
        endif ()
    else ()
        if (EMDEVIF_USE_CPP_MODULES)
            target_link_libraries(${PROJECT_NAME} PUBLIC freertos_kernel)
        else ()
            target_link_libraries(${PROJECT_NAME} INTERFACE freertos_kernel)
        endif ()
    endif ()

    file(GLOB emdevifSystemFreeRtosImplSrcs "Implements/FreeRTOS/*.cpp")
    if (EMDEVIF_USE_CPP_MODULES)

        target_include_directories(${PROJECT_NAME} PUBLIC Implements/FreeRTOS/inc)

        target_sources(${PROJECT_NAME} PRIVATE ${emdevifSystemFreeRtosImplSrcs})

        file(GLOB emdevifSystemFreeRtosImplModules "Implements/FreeRTOS/modules/*.cppm")
        target_sources(${PROJECT_NAME} PUBLIC
            FILE_SET emdevif_freertos_impl_modules TYPE CXX_MODULES FILES
                ${emdevifSystemFreeRtosImplModules}
        )
    else ()
        target_include_directories(${PROJECT_NAME} INTERFACE Implements/FreeRTOS/inc)

        add_library(emdevif_system_freertos_impl_ OBJECT)
        target_sources(emdevif_system_freertos_impl_ PRIVATE ${emdevifSystemFreeRtosImplSrcs})
        target_link_libraries(emdevif_system_freertos_impl_
            PUBLIC emdevif_core
            PRIVATE emdevif_compile_options emdevif_system
        )

        target_link_libraries(${PROJECT_NAME} INTERFACE
            $<TARGET_OBJECTS:emdevif_system_freertos_impl_>
        )
    endif ()
else ()
    if (NOT (TARGET emdevif_system_impl))
        message(FATAL_ERROR "If EMDEVIF_THREAD_SYSTEM not equal to FreeRTOS, it requires \
target `emdevif_system_impl\' to implements system interface.")
    endif ()

    target_link_libraries(${PROJECT_NAME} PUBLIC emdevif_system_impl)
endif ()

set(additionalMessage "")
if ((EMDEVIF_THREAD_SYSTEM STREQUAL "FreeRTOS") AND (EMDEVIF_FREERTOS_USE_STM32CUBEMX))
    set(additionalMessage " Use STM32CubeMX generated FreeRTOS lib.")
endif ()
message(STATUS "[${PROJECT_NAME}]: Use \"${EMDEVIF_THREAD_SYSTEM}\".${additionalMessage}")
