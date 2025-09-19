/**
 * @file emdevif_user_declares.cppm
 * @author DuYicheng
 * @date 2025-09-04
 * @brief 实现当前平台的设备句柄键值对
 */

module;

#include <string_view>

#include "usart.h"

#include "emdevif/fatal_handler.hpp"

export module emdevif.userDeclares;

import emdevif.container.map;
import emdevif.sys.mutex;
import emdevif.errorHandler;

export namespace emdevif::user_declares {

constexpr auto peripheral_handle_map = makeStaticMap<std::string_view, void*>({{"test transmit serial", &huart6}});

namespace logger {

Mutex mutex;

void init() noexcept
{
    mutex = Mutex::create({.name = "Logger Mutex"});
    if (!mutex.getHandle().has_value()) {
        EMDEVIF_FATAL_HANDLER("Failed to create `Logger Mutex\'!");
    }
}

void lock() noexcept
{
    mutex.lock();
}

void unlock() noexcept
{
    mutex.unlock();
}

std::size_t getTimeLine() noexcept
{
    constinit static std::size_t timeLine = 0;
    return timeLine++;
}

}  // namespace logger

}  // namespace emdevif::user_declares
