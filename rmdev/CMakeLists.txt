cmake_minimum_required(VERSION 3.28...4.0.2)

project(rmdev C CXX)

if (NOT (TARGET emdevif))
    message(FATAL_ERROR "[${PROJECT_NAME}]: emdevif target not found! Please add emdevif as a subdirectory before adding rmdev.")
endif ()

option(RMDEV_ENABLE_INS_MODULE "Enable the rmdev_ins module." OFF)

set(RMDEV_ENABLED_DRIVER_LIST "" CACHE STRING "Select which device driver to use.")

if (EMDEVIF_USE_STM32CUBEMX)
    enable_language(ASM)
endif ()

if (EMDEVIF_USE_CPP_MODULES)
    message(STATUS "[${PROJECT_NAME}]: \"EMDEVIF_USE_CPP_MODULES\" was set to ON, so ${PROJECT_NAME} used too.")
endif ()

add_subdirectory(modules/math)
add_subdirectory(modules/control_algorithm)
add_subdirectory(modules/kinematic_solution)
add_subdirectory(modules/device_model)
add_subdirectory(modules/debug_assistance)
add_subdirectory(modules/message_manager)

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME} INTERFACE
    emdevif

    rmdev_math
    rmdev_control_algorithm
    rmdev_kinematic_solution
    rmdev_device_model
    rmdev_debug_assistance
    rmdev_message_manager
)

if (NOT (RMDEV_ENABLED_DRIVER_LIST STREQUAL ""))
    block()
        foreach (usedDriverName IN LISTS RMDEV_ENABLED_DRIVER_LIST)
            set(libName "rmdev_driver_${usedDriverName}")

            add_subdirectory(drivers/${usedDriverName})
            target_link_libraries(${PROJECT_NAME} INTERFACE ${libName})

            message(STATUS "[${PROJECT_NAME}]: Enabled driver lib `${libName}`")
        endforeach ()
    endblock()
endif ()

if (RMDEV_ENABLE_INS_MODULE)
    add_subdirectory(modules/ins)
    target_link_libraries(${PROJECT_NAME} INTERFACE rmdev_ins)
    message(STATUS "[${PROJECT_NAME}]: Enabled \"INS\" lib.")
endif ()
