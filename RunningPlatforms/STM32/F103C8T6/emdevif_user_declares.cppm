/**
 * @file emdevif_user_declares.cppm
 * @brief emdevif 由用户实现的功能
 */

module;

#include <cstdint>

#include <limits>
#include <string_view>
#include <span>

#include "printf.h"

#include "main.h"
#include "usart.h"

#include "emdevif/core/fatal_handler.h"

export module emdevif.user_declares;

import emdevif.core.data_container.array_map;
import emdevif.core.error_handler;

import emdevif.peripheral.model.serial;

export namespace test {

EMDEVIF_NO_RETURN void terminateImpl() noexcept
{
    __disable_irq();
    while (true) {
    }
}

}  // namespace test

namespace emdevif::user_declares {

constinit emdevif::SerialModel::Instance test_transmit_serial_model{
    .handle = USART1,
    .transmit_function = [](bool, void* handle, std::span<const uint8_t> data, uint32_t) -> emdevif::ErrorCode {
        for (const auto e : data) {
            LL_USART_TransmitData8(static_cast<::USART_TypeDef*>(handle), e);
        }

        return ErrorCode::Success;
    }};

export constexpr auto peripheral_handle_map =
    makeStaticMap<std::string_view, void*>({{"test transmit serial", &test_transmit_serial_model}});

export namespace timeline {

inline uint64_t getMicroseconds() noexcept
{
    return 0;
}

}  // namespace timeline

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
