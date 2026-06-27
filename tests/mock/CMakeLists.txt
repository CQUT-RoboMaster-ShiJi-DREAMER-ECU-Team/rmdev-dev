cmake_minimum_required(VERSION 3.31...4.2.0)

# Non-GoogleTest tests (independent executables)
add_subdirectory(emdevif/core/error_handler)

# GoogleTest via CPM
CPMFindPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG v1.17.0
    VERSION 1.17.0
    OPTIONS "BUILD_GMOCK OFF" "INSTALL_GTEST OFF"
)
if (NOT googletest_ADDED)
    message(FATAL_ERROR "CPM: Failed to add package `googletest`")
endif ()

project(host_gtest LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    # emdevif core
    emdevif/core/concepts_test.cpp
    emdevif/core/data_container/ArrayMap_test.cpp
    emdevif/core/data_container/FixedString_test.cpp
    emdevif/core/data_container/MessageQueue_test.cpp
    emdevif/core/data_container/RingBuffer_test.cpp
    emdevif/core/endian_test.cpp
    emdevif/core/integer_suffix_test.cpp
    emdevif/core/resource_guard/resource_guard_test.cpp
    emdevif/core/type_traits/type_traits_test.cpp
    emdevif/core/utils/BitInt_test.cpp
    emdevif/core/utils/InitOnce_test.cpp
    emdevif/core/utils/When_test.cpp

    # emdevif timeline (link-time injection)
    emdevif/timeline/user_impl_timeline.cpp
    emdevif/timeline/Timeline_test.cpp

    # emdevif peripheral (link-time injection)
    emdevif/peripheral/user_impl_peripheral.cpp
    emdevif/peripheral/PeripheralHandleMap_test.cpp

    # rmdev
    rmdev/debug_assistance/VOFA_test.cpp
    rmdev/message_manager/Subscriber_test.cpp
    rmdev/message_manager/Topic_test.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_precompile_headers(${PROJECT_NAME} PRIVATE host_test_pch.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    GTest::gtest_main

    emdevif_core
    emdevif_timeline
    emdevif_peripheral

    rmdev_message_manager
    rmdev_debug_assistance
)

# CTest registration
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
set_tests_properties(${PROJECT_NAME} PROPERTIES DEPENDS emdevif_core_error_handler_test)

addCopyFinallyBinaryFileTarget(${PROJECT_NAME})
