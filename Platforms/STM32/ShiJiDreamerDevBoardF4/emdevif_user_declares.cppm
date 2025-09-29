/**
 * @file emdevif_user_declares.cppm
 * @author DuYicheng
 * @date 2025-09-04
 * @brief emdevif 由用户实现的功能
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

constexpr auto peripheral_handle_map = makeStaticMap<std::string_view, void*>({{"test transmit serial", &huart1}});

namespace logger {

inline std::size_t getTimeLine() noexcept
{
    constinit static std::size_t timeLine = 0;
    return timeLine++;
}

static char buffer[512];
static std::size_t buffer_head = 0;
char* getBuffer() noexcept
{
    return buffer;
}

ErrorCode printLogMessage(const char* message) noexcept
{
    const auto ret = ::snprintf(buffer + buffer_head, std::size(buffer) - buffer_head, "%s", message);
    if (ret < 0) {
        EMDEVIF_FATAL_HANDLER("Failed to print log message");
    }

    buffer_head += ret;

    return ErrorCode::Success;
}

}  // namespace logger

}  // namespace emdevif::user_declares
