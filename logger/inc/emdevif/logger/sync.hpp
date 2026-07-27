/**
 * @file sync.hpp
 * @brief 同步模式日志公共声明
 */

#pragma once
#ifndef EMDEVIF_LOGGER_SYNC_HPP
#define EMDEVIF_LOGGER_SYNC_HPP

#include <cstdarg>
#include <cstddef>

#include "emdevif/core/fatal_handler.h"
#include "emdevif/core/error_handler.hpp"
#include "emdevif/logger/config.hpp"

namespace emdevif::logger::detail {

namespace sync {

ErrorCode logInit(VsnprintfImpl vsprintf_impl) noexcept;
void logDeInit() noexcept;
void logImpl(LoggerLevel level, const char* format, std::va_list args) noexcept;

}  // namespace sync

inline ErrorCode logInit(const VsnprintfImpl vsprintf_impl) noexcept
{
    return sync::logInit(vsprintf_impl);
}

inline void logDeInit() noexcept
{
    sync::logDeInit();
}

inline void logImpl(const LoggerLevel level, const char* format, std::va_list args) noexcept
{
    sync::logImpl(level, format, args);
}

}  // namespace emdevif::logger::detail

namespace emdevif::user_impl::logger {

std::size_t getTimeLine() noexcept;
emdevif::ErrorCode printLogMessage(const char* message) noexcept;

}  // namespace emdevif::user_impl::logger

#include "emdevif/logger/sync_async_interface.hpp"

#endif  // !EMDEVIF_LOGGER_SYNC_HPP
