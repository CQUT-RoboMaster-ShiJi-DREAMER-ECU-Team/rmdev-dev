idf_component_register(
    SRCS "main.cpp"
    PRIV_REQUIRES spi_flash esp_driver_uart
    INCLUDE_DIRS ""
)

include("${EMDEVIF_TEST_TEST_SOURCE_DIR_}/testForceToIgnore.cmake")

message(DEBUG "[${PROJECT_NAME}, component name: ${COMPONENT_NAME}]: Before call function `testForceToIgnore\': "
    "EMDEVIF_TEST_IGNORE_NAME_LIST=\"${EMDEVIF_TEST_IGNORE_NAME_LIST}\""
)
list(APPEND CMAKE_MESSAGE_INDENT " >> ")

testForceToIgnore("${EMDEVIF_TEST_IGNORE_NAME_LIST}" EMDEVIF_TEST_IGNORE_NAME_LIST "loggerTest")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(DEBUG "[${PROJECT_NAME}, component name: ${COMPONENT_NAME}]: After call function `testForceToIgnore\': "
    "EMDEVIF_TEST_IGNORE_NAME_LIST=\"${EMDEVIF_TEST_IGNORE_NAME_LIST}\""
)

set(EMDEVIF_GNU_COMPILE_OPTIONS "-Wall;-Wextra")
set(EMDEVIF_ENABLE_TESTS ON)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif
    ${CMAKE_BINARY_DIR}/emdevif_collection/emdevif
)

target_compile_definitions(emdevif_system PUBLIC EMDEVIF_SYS_TICK_TYPE_UINT_BITS=32)

set(BUILD_RMDEV OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/TestImplement ${CMAKE_BINARY_DIR}/TestImplement)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/emdevif_user_declares)

target_link_libraries(${COMPONENT_LIB} PRIVATE emdevif test_implement)
