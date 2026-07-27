cmake_minimum_required(VERSION 3.28...4.0.2)

project(emdevif_logger C CXX)

set(EMDEVIF_LOGGER_MODE 0 CACHE STRING "Logger mode: 0=Sync, 1=Async, 2=ExternalImpl")
set_property(CACHE EMDEVIF_LOGGER_MODE PROPERTY STRINGS 0 1 2)

set(EMDEVIF_LOGGER_LINK_SYSTEM_LIB ON CACHE BOOL
    "Enabled to link emdevif_system library to emdevif_logger, required for async or sync with lock"
)

if (EMDEVIF_LOGGER_MODE STREQUAL "0")
    add_subdirectory(sync)
    add_library(emdevif_logger ALIAS emdevif_logger_sync)
elseif (EMDEVIF_LOGGER_MODE STREQUAL "1")
    add_subdirectory(async)
    add_library(emdevif_logger ALIAS emdevif_logger_async)
elseif (EMDEVIF_LOGGER_MODE STREQUAL "2")
    add_subdirectory(external)
    add_library(emdevif_logger ALIAS emdevif_logger_external)
else()
    message(FATAL_ERROR "Invalid EMDEVIF_LOGGER_MODE value! It should be 0 (Sync), 1 (Async) or 2 (ExternalImpl).")
endif()
