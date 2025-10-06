/**
 * @file emdevif_user_declares.cppm
 * @author DuYicheng
 * @date 2025-09-04
 * @brief emdevif 由用户实现的功能
 */

module;

#include <string_view>
#include <limits>

#include "printf.h"

#include "usart.h"
#include "tim.h"

#include "emdevif/fatal_handler.hpp"

export module emdevif.userDeclares;

import emdevif.container.map;
import emdevif.errorHandler;

import emdevif.sys.atomic;

export namespace emdevif::user_declares {

constexpr auto peripheral_handle_map = makeStaticMap<std::string_view, void*>(
    {{"test transmit serial", &huart6}, {"INS result transmit serial", &huart1}});

namespace timeline {

emdevif::atomic<uint32_t> overflow_count = 0;

extern "C" void TIM5_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM5)) {
        overflow_count.fetch_add(1, emdevif::memory_order::release);

        LL_TIM_ClearFlag_UPDATE(TIM5);
    }
}

inline uint64_t getMicroseconds() noexcept
{
    return static_cast<uint64_t>(overflow_count.load(emdevif::memory_order::acquire)) *
               static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) +
           static_cast<uint64_t>(LL_TIM_GetCounter(TIM5));
}

}  // namespace timeline

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
