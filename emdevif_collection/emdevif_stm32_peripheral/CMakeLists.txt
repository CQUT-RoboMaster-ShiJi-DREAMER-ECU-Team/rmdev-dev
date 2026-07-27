cmake_minimum_required(VERSION 3.28...4.0.2)

set(EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST "" CACHE STRING
    "Add enabled peripheral in this list. For example, set it to \"USART;CAN\" in order to \
enable USART and CAN peripheral"
)

set(EMDEVIF_STM32_PERIPHERAL_DRIVER "" CACHE STRING
    "Set witch driver(HAL, LL or BOTH) to use in EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST"
)

project(emdevif_stm32_peripheral C CXX ASM)

if (EMDEVIF_USE_CPP_MODULES)
    add_library(${PROJECT_NAME} STATIC)
else ()
    add_library(${PROJECT_NAME} INTERFACE)
endif ()

function(chooseFilesToInclude peripheralList driverList out_chosenFiles out_chosenHeaders)
    set(chosenFiles "")
    set(chosenHeaders "")

    set(headersBaseDir "inc/emdevif/stm32_peripheral")

    list(LENGTH peripheralList peripheralListLength)
    list(LENGTH driverList     driverListLength)
    if (NOT (peripheralListLength EQUAL driverListLength))
        message(FATAL_ERROR "The length of EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST and EMDEVIF_STM32_PERIPHERAL_DRIVER \
are not equal. Please make them corresponding to each element one by one.")
    endif ()

    list(FIND driverList "HAL" firstHalDriverElementIndex)
    list(FIND driverList "BOTH" firstBothElementIndex)
    if ((NOT (firstHalDriverElementIndex EQUAL -1)) OR (NOT (firstBothElementIndex EQUAL -1)))
        list(APPEND chosenHeaders "${headersBaseDir}/hal/detail/hal_status_mapper.hpp")
    endif ()

    math(EXPR maxIndex "${peripheralListLength} - 1")
    foreach (index RANGE ${maxIndex})
        list(GET driverList     ${index} driver)
        list(GET peripheralList ${index} peripheral)

        string(TOLOWER ${peripheral} realPeripheralName)

        if (driver STREQUAL "HAL")
            list(APPEND chosenFiles "STM32_HAL_Driver/${realPeripheralName}_hal_impl.cppm")
            list(APPEND chosenHeaders "${headersBaseDir}/hal/${realPeripheralName}.hpp")
            set(messageDriverName "\"HAL\"")
        elseif (driver STREQUAL "LL")
            list(APPEND chosenFiles "STM32_LL_Driver/${realPeripheralName}_ll_impl.cppm")
            list(APPEND chosenHeaders "${headersBaseDir}/ll/${realPeripheralName}.hpp")
            set(messageDriverName "\"LL\"")
        elseif (driver STREQUAL "BOTH")
            list(APPEND chosenFiles "STM32_HAL_Driver/${realPeripheralName}_hal_impl.cppm")
            list(APPEND chosenFiles "STM32_LL_Driver/${realPeripheralName}_ll_impl.cppm")

            list(APPEND chosenHeaders "${headersBaseDir}/hal/${realPeripheralName}.hpp")
            list(APPEND chosenHeaders "${headersBaseDir}/ll/${realPeripheralName}.hpp")
            set(messageDriverName "\"HAL\" and \"LL\"")
        else ()
            message(FATAL_ERROR "Unknown item(named \"${driver}\") in EMDEVIF_STM32_PERIPHERAL_DRIVER!")
        endif ()

        message(STATUS "[${PROJECT_NAME}]: Enabled peripheral \"${peripheral}\" with driver ${messageDriverName}")
    endforeach ()

    set(${out_chosenFiles} ${chosenFiles} PARENT_SCOPE)
    set(${out_chosenHeaders} ${chosenHeaders} PARENT_SCOPE)
endfunction()

chooseFilesToInclude(
    "${EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST}"
    "${EMDEVIF_STM32_PERIPHERAL_DRIVER}"
    emdevifStm32HalDriverImplModules
    emdevifStm32HalDriverImplHeaders
)

if (EMDEVIF_USE_CPP_MODULES)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC emdevif_core
        PRIVATE emdevif_compile_options emdevif_stm32cubemx_support emdevif_peripheral_model
    )

    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET emdevif_stm32_peripheral_modules TYPE CXX_MODULES
        FILES ${emdevifStm32HalDriverImplModules}

        PUBLIC FILE_SET emdevif_stm32_peripheral_headers TYPE HEADERS BASE_DIRS inc
        FILES ${emdevifStm32HalDriverImplHeaders}
    )
else ()
    target_link_libraries(${PROJECT_NAME} INTERFACE emdevif_core emdevif_compile_options
        emdevif_stm32cubemx_support emdevif_peripheral_model
    )

    target_sources(${PROJECT_NAME} INTERFACE
        FILE_SET emdevif_stm32_peripheral_headers TYPE HEADERS BASE_DIRS inc
        FILES ${emdevifStm32HalDriverImplHeaders}
    )
endif ()
