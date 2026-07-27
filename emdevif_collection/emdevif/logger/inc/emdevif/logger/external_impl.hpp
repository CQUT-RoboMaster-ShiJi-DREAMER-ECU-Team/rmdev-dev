/**
 * @file external_impl.hpp
 * @brief 外部实现模式日志公共声明
 */

#pragma once
#ifndef EMDEVIF_LOGGER_EXTERNAL_IMPL_HPP
#define EMDEVIF_LOGGER_EXTERNAL_IMPL_HPP

#include <cstdarg>

#include "emdevif/core/fatal_handler.h"
#include "emdevif/core/error_handler.hpp"
#include "emdevif/logger/config.hpp"

namespace emdevif::user_impl::logger {

void logImpl(const int level, const char* format, std::va_list args) noexcept;
void logVerboseImpl(const char* format, std::va_list args) noexcept;
void logDebugImpl(const char* format, std::va_list args) noexcept;
void logInfoImpl(const char* format, std::va_list args) noexcept;
void logWarningImpl(const char* format, std::va_list args) noexcept;
void logErrorImpl(const char* format, std::va_list args) noexcept;
void logFatalImpl(const char* format, std::va_list args) noexcept;

}  // namespace emdevif::user_impl::logger

namespace emdevif::logger {

inline void log(const LoggerLevel level, const char* format, ...) noexcept
{
    if (level < logger_ignore_level) {
        return;
    }

    std::va_list args;
    va_start(args, format);
    ::emdevif::user_impl::logger::logImpl(static_cast<int>(level), format, args);
    va_end(args);
}

inline void verbose(const char* format, ...) noexcept
{
    if constexpr (constexpr auto level = LoggerLevel::Verbose; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        ::emdevif::user_impl::logger::logVerboseImpl(format, args);
        va_end(args);
    }
}

inline void debug(const char* format, ...) noexcept
{
    if constexpr (constexpr auto level = LoggerLevel::Debug; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        ::emdevif::user_impl::logger::logDebugImpl(format, args);
        va_end(args);
    }
}

inline void info(const char* format, ...) noexcept
{
    if constexpr (constexpr auto level = LoggerLevel::Info; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        ::emdevif::user_impl::logger::logInfoImpl(format, args);
        va_end(args);
    }
}

inline void warning(const char* format, ...) noexcept
{
    if constexpr (constexpr auto level = LoggerLevel::Warning; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        ::emdevif::user_impl::logger::logWarningImpl(format, args);
        va_end(args);
    }
}

inline void error(const char* format, ...) noexcept
{
    if constexpr (constexpr auto level = LoggerLevel::Error; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        ::emdevif::user_impl::logger::logErrorImpl(format, args);
        va_end(args);
    }
}

inline void fatal(const char* format, ...) noexcept
{
    if constexpr (constexpr auto level = LoggerLevel::Fatal; !(level < logger_ignore_level)) {
        std::va_list args;
        va_start(args, format);
        ::emdevif::user_impl::logger::logFatalImpl(format, args);
        va_end(args);

        EMDEVIF_FATAL_HANDLER("Fatal log message outputted, terminating program");
    }
}

}  // namespace emdevif::logger

#endif  // !EMDEVIF_LOGGER_EXTERNAL_IMPL_HPP
