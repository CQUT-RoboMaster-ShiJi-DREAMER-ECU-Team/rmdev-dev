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
#include "tim.h"

#include "emdevif/fatal_handler.hpp"

export module emdevif.userDeclares;

import emdevif.container.map;
import emdevif.errorHandler;

import emdevif.stm32Peripheral.hal.pwm;

namespace emdevif::user_declares {

constinit emdevif::stm32hal::PwmHandle breathing_light_pwm_handle{.htim = &htim3, .channel = TIM_CHANNEL_1};

export constexpr auto peripheral_handle_map = makeStaticMap<std::string_view, void*>(
    {{"test transmit serial", &huart1}, {"breathing light pwm", &breathing_light_pwm_handle}});

export namespace logger {

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
