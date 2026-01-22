cmake_minimum_required(VERSION 3.28...4.0.2)

option(BUILD_RMDEV "Enabled it to build `rmdev\' and run the test for rmdev." ON)
option(ENABLE_AFTER_UNIT_TEST_DEMO "Enable it to link lib \"AfterUnitTestDemo\" \
to run demo program in this platform." OFF
)

project(test_implement C CXX)

add_library(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME} PRIVATE
    emdevif emdevif_unit_test emdevif_user_declares
    emdevif_test_framework printf
)
if (BUILD_RMDEV)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        rmdev rmdev_unit_test
    )
    target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_RMDEV=1)

    message(STATUS "[${PROJECT_NAME}]: Will build and run the test for rmdev.")
endif ()

if (ENABLE_AFTER_UNIT_TEST_DEMO)
    target_link_libraries(${PROJECT_NAME} PRIVATE AfterUnitTestDemo)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_AFTER_UNIT_TEST_DEMO=1)
    message(STATUS "[${PROJECT_NAME}]: Enabled demo.")
endif ()

if (DEFINED PLATFORM_NAME)
    if (PLATFORM_NAME STREQUAL "esp32")
        idf_component_get_property(espIdfLogLibName log COMPONENT_ALIAS)
        message(STATUS "[${PROJECT_NAME}]: espIdfLogLibName=\"${espIdfLogLibName}\"")
        target_link_libraries(${PROJECT_NAME} PRIVATE ${espIdfLogLibName})
    endif ()
    
    target_sources(${PROJECT_NAME}
        PRIVATE "${PLATFORM_NAME}_test_impl.cpp"

        PUBLIC FILE_SET header_file TYPE HEADERS FILES rmdev_test.h
    )
endif ()
