idf_component_register(
    SRCS "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
    PRIV_REQUIRES spi_flash esp_driver_uart
    INCLUDE_DIRS ""
)

set(EMDEVIF_GNU_COMPILE_OPTIONS "-Wall;-Wextra" CACHE STRING "" FORCE)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif
    ${CMAKE_BINARY_DIR}/emdevif_collection/emdevif
)

target_compile_definitions(emdevif_system PUBLIC EMDEVIF_SYS_TICK_TYPE_UINT_BITS=32)
target_compile_definitions(emdevif_logger PUBLIC EMDEVIF_LOGGER_DYNAMIC_CREATE=1)

set(BUILD_RMDEV OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/rmdev_test ${CMAKE_BINARY_DIR}/rmdev_test)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/emdevif_user_declares)

target_link_libraries(${COMPONENT_LIB} PRIVATE emdevif rmdev_test)
