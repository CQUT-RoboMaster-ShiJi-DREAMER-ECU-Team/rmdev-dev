
module;

#include <cstdint>
#include <cstdio>

#include <iterator>
#include <string_view>
#include <span>

#include "driver/uart.h"
#include "sdkconfig.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "esp_log.h"

#include "emdevif/fatal_handler.h"
#include "emdevif/attributes_and_useful_macros.h"

export module emdevif.userDeclares;

import emdevif.errorHandler;
import emdevif.container.map;
import emdevif.peripheral.model.serial;

export namespace test {

EMDEVIF_NO_RETURN void terminateImpl() noexcept
{
    ESP_LOGW("emdevif", "Terminate function called.");

    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    esp_deep_sleep_start();
}

}  // namespace test

export namespace emdevif::user_declares {

uart_port_t test_tx_uart = UART_NUM_2;
emdevif::ErrorCode esp32_uart_transmit(bool, void* handle, std::span<const uint8_t> data, uint32_t)
{
    auto huart = *static_cast<uart_port_t*>(handle);
    constexpr auto log_tag = "emdevif::user_declares esp32_uart_transmit";

    ESP_LOGV(log_tag, "Pre: huart=%d, data.size_bytes()=%zu", static_cast<int>(huart), data.size_bytes());
    const auto ret = uart_write_bytes(huart, data.data(), data.size_bytes());
    if (ret < 0) {
        ESP_LOGW(log_tag, "Transmit failed. ErrorCode=%d", ret);
        return ErrorCode::OperationFail;
    }

    ESP_LOGV(log_tag, "Transmit succeed.");
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
    return 0;
}

char* getBuffer() noexcept
{
    return nullptr;
}

ErrorCode printLogMessage(const char* message) noexcept
{
    return ErrorCode::NotImplemented;
}

}  // namespace logger

}  // namespace emdevif::user_declares
