/**
 * @file sync_async_interface.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_LOGGER_SYNC_ASYNC_INTERFACE_HPP
#define EMDEVIF_LOGGER_SYNC_ASYNC_INTERFACE_HPP

#include <cstdarg>

#include <utility>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/logger/config.hpp"

namespace emdevif::logger {

inline emdevif::ErrorCode init(const VsnprintfImpl vsprintf_impl) noexcept
{
    return detail::logInit(vsprintf_impl);
}

inline void deInit() noexcept
{
    detail::logDeInit();
}

inline void log(const LoggerLevel level, const char* format, ...) noexcept
{
    if (level < logger_ignore_level) {
        return;
    }

    std::va_list args;
    va_start(args, format);
    detail::logImpl(level, format, args);
    va_end(args);
}

inline void verbose(const char* format, ...) noexcept
{
    // ReSharper disable once CppRedundantComplexityInComparison
    if constexpr (constexpr auto level = LoggerLevel::Verbose; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        detail::logImpl(level, format, args);
        va_end(args);
    }
}

inline void debug(const char* format, ...) noexcept
{
    // ReSharper disable once CppRedundantComplexityInComparison
    if constexpr (constexpr auto level = LoggerLevel::Debug; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        detail::logImpl(level, format, args);
        va_end(args);
    }
}

inline void info(const char* format, ...) noexcept
{
    // ReSharper disable once CppRedundantComplexityInComparison
    if constexpr (constexpr auto level = LoggerLevel::Info; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        detail::logImpl(level, format, args);
        va_end(args);
    }
}

inline void warning(const char* format, ...) noexcept
{
    // ReSharper disable once CppRedundantComplexityInComparison
    if constexpr (constexpr auto level = LoggerLevel::Warning; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        detail::logImpl(level, format, args);
        va_end(args);
    }
}

inline void error(const char* format, ...) noexcept
{
    // ReSharper disable once CppRedundantComplexityInComparison
    if constexpr (constexpr auto level = LoggerLevel::Error; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        detail::logImpl(level, format, args);
        va_end(args);
    }
}

inline void fatal(const char* format, ...) noexcept
{
    // ReSharper disable once CppRedundantComplexityInComparison
    if constexpr (constexpr auto level = LoggerLevel::Fatal; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        detail::logImpl(level, format, args);
        va_end(args);

        EMDEVIF_FATAL_HANDLER("Fatal log message outputted, terminating program");
    }
}

}  // namespace emdevif::logger

#endif  // !EMDEVIF_LOGGER_SYNC_ASYNC_INTERFACE_HPP
