
module;

#include <cstdint>
#include <cstdio>

#include <iterator>
#include <string_view>
#include <span>

#include "driver/uart.h"
#include "sdkconfig.h"
#include "emdevif/fatal_handler.h"

export module emdevif.userDeclares;

import emdevif.errorHandler;
import emdevif.container.map;
import emdevif.peripheral.model.serial;

export namespace test {

// todo 改名，改为 terminate
void disableIrq() noexcept
{
    esp_system_abort("emdevif: Terminate function called.");
}

}  // namespace test

export namespace emdevif::user_declares {

uart_port_t test_tx_uart = UART_NUM_2;
emdevif::ErrorCode esp32_uart_transmit(bool, void* handle, std::span<const uint8_t> data, uint32_t)
{
    auto huart = static_cast<uart_port_t*>(handle);
    const auto ret = uart_write_bytes(*huart, data.data(), data.size_bytes());
    if (ret < 0) {
        return ErrorCode::OperationFail;
    }
    return ErrorCode::Success;
}
emdevif::SerialModel::Instance test_tx_uart_model{.handle_ = &test_tx_uart, .transmit_function_ = esp32_uart_transmit};

constexpr auto peripheral_handle_map =
    emdevif::makeStaticMap<std::string_view, void*>({{"test transmit serial", &test_tx_uart_model}});

namespace timeline {

inline uint64_t getMicroseconds() noexcept
{
    return 0;  // 暂时不实现
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
    const auto ret = std::snprintf(buffer + buffer_head, std::size(buffer) - buffer_head, "%s", message);
    if (ret < 0) {
        EMDEVIF_FATAL_HANDLER("Failed to print log message");
    }

    buffer_head += ret;

    return ErrorCode::Success;
}

}  // namespace logger

}  // namespace emdevif::user_declares
