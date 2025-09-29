/**
 * @file emdevif_user_declares.cppm
 * @author DuYicheng
 * @date 2025-09-04
 * @brief 实现当前平台的设备句柄键值对
 */

module;

#include <string_view>

#include "printf.h"

#include "usart.h"

#include "emdevif/fatal_handler.hpp"

export module emdevif.userDeclares;

import emdevif.container.map;
import emdevif.errorHandler;

export namespace emdevif::user_declares {

constexpr auto peripheral_handle_map = makeStaticMap<std::string_view, void*>({{"test transmit serial", &huart6}});

namespace logger {

inline std::size_t getTimeLine() noexcept
{
    constinit static std::size_t timeLine = 0;
    return timeLine++;
}

static char buffer[256];
char* getBuffer() noexcept
{
    return buffer;
}

ErrorCode printLogMessage(const char* message) noexcept
{
    const auto ret = ::sprintf(buffer, "%s", message);
    if (ret < 0) {
        EMDEVIF_FATAL_HANDLER("Failed to print log message");
    }

    return ErrorCode::Success;
}

}  // namespace logger

}  // namespace emdevif::user_declares
