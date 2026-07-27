/**
 * @file config_values.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_LOGGER_CONFIG_VALUES_HPP
#define EMDEVIF_LOGGER_CONFIG_VALUES_HPP

#include <cstdarg>
#include <cstddef>
#include <cstdint>

#include "emdevif/logger/config.hpp"

namespace emdevif::logger {

#if (EMDEVIF_LOGGER_MODE == 0 || EMDEVIF_LOGGER_MODE == 1)
using VsnprintfImpl = int (*)(char* dest, std::size_t buffer_size, const char* format, std::va_list args);
#endif

/// @brief 日志工作模式枚举。
enum class LoggerMode : uint_fast8_t {
    Sync = 0,
    Async = 1,
    ExternalImpl = 2
};

// ReSharper disable once CppRedundantCastExpression
/// @brief 当前配置的日志工作模式。
constexpr auto logger_mode = static_cast<LoggerMode>(EMDEVIF_LOGGER_MODE);

/// @brief 日志等级枚举。
enum class LoggerLevel : int_fast8_t {
    Verbose = 0,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

// ReSharper disable once CppRedundantCastExpression
/// @brief 当前配置的日志忽略等级，低于此等级的日志将被过滤。
constexpr auto logger_ignore_level = static_cast<LoggerLevel>(EMDEVIF_LOGGER_IGNORE_LEVEL);

static_assert(logger_ignore_level >= LoggerLevel::Verbose && logger_ignore_level <= LoggerLevel::Fatal,
              "Invalid EMDEVIF_LOGGER_IGNORE_LEVEL value! It should be a value in enum class `emdevif::LoggerLevel`.");

/**
 * @brief 将日志等级枚举值转换为字符串。
 * @param logger_level 日志等级。
 * @return 对应的 C 风格字符串表示。
 */
constexpr const char* toCString(const LoggerLevel logger_level) noexcept
{
    switch (logger_level) {
    case LoggerLevel::Verbose:
        return "Verbose";
    case LoggerLevel::Debug:
        return "Debug";
    case LoggerLevel::Info:
        return "Info";
    case LoggerLevel::Warning:
        return "Warning";
    case LoggerLevel::Error:
        return "Error";
    case LoggerLevel::Fatal:
        return "Fatal";
    default:
        return "Unknown";
    }
}

#if (EMDEVIF_LOGGER_MODE == 0 || EMDEVIF_LOGGER_MODE == 1)
// ReSharper disable once CppRedundantCastExpression
/// @brief 当前配置的单条日志缓冲区大小（字节）。
constexpr auto logger_buffer_size = static_cast<std::size_t>(EMDEVIF_LOGGER_BUFFER_SIZE);

// ReSharper disable once CppRedundantCastExpression
/// @brief 当前配置的日志缓冲区数量。
constexpr auto logger_buffer_count = static_cast<std::size_t>(EMDEVIF_LOGGER_BUFFER_COUNT);
#endif

}  // namespace emdevif::logger

#endif  // !EMDEVIF_LOGGER_CONFIG_VALUES_HPP
