/**
 * @file user_declares.hpp
 * @brief emdevif 由用户实现的功能
 */

#pragma once

#include <string_view>
#include <limits>
#include <tuple>

#include "emdevif/core/fatal_handler.h"

#include "main.h"
#include "usart.h"
#include "tim.h"
#include "spi.h"
#include "gpio.h"

#include "emdevif/core/data_container/array_map.hpp"
#include "emdevif/core/error_handler.hpp"
#include "emdevif/system/atomic.hpp"

#include "emdevif/peripheral/model/gpio.hpp"
#include "emdevif/peripheral/model/pwm.hpp"
#include "emdevif/peripheral/model/serial.hpp"
#include "emdevif/peripheral/model/spi.hpp"

#include "emdevif/stm32_peripheral/hal/pwm.hpp"
#include "emdevif/stm32_peripheral/hal/gpio.hpp"
#include "emdevif/stm32_peripheral/hal/usart.hpp"
#include "emdevif/stm32_peripheral/hal/spi.hpp"

#include "printf.h"

namespace test {

EMDEVIF_NO_RETURN inline void terminateImpl() noexcept
{
    __disable_irq();
    while (true) {
    }
}

}  // namespace test

namespace emdevif::user_declares {

inline constinit emdevif::SerialModel::Instance test_transmit_serial_model{
    .handle = &huart6,
    .transmit_function = emdevif::stm32hal::uartTransmitBlocking};

inline constinit emdevif::SerialModel::Instance ins_result_transmit_serial_model{
    .handle = &huart1,
    .get_state_function = emdevif::stm32hal::uartGetState,
    .transmit_function = emdevif::stm32hal::uartTransmitBlocking};

inline constinit emdevif::stm32hal::PwmHandle bmi088_heat_pwm_handle{&htim10, TIM_CHANNEL_1};
inline constinit emdevif::PwmModel::Instance bmi088_heat_pwm_model{.handle = &bmi088_heat_pwm_handle,
                                                                   .enable = emdevif::stm32hal::pwmEnable,
                                                                   .disable = emdevif::stm32hal::pwmDisable,
                                                                   .setRatio = emdevif::stm32hal::pwmSetRatio};

inline constinit emdevif::SpiModel::Instance bmi088_comm_spi_model{
    .handle = &hspi1,
    .transmit_receive_function = emdevif::stm32hal::spiTransmitReceiveBlock};

inline emdevif::stm32hal::GpioHandle bmi088_accel_cs_gpio{CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin};
inline constinit emdevif::GpioModel::Instance bmi088_accel_cs_gpio_model{.handle = &bmi088_accel_cs_gpio,
                                                                         .write_function = stm32hal::gpioWrite};

inline emdevif::stm32hal::GpioHandle bmi088_gyro_cs_gpio{CS1_GYRO_GPIO_Port, CS1_GYRO_Pin};
inline constinit emdevif::GpioModel::Instance bmi088_gyro_cs_gpio_model{.handle = &bmi088_gyro_cs_gpio,
                                                                        .write_function = stm32hal::gpioWrite};

constexpr auto peripheral_handle_map =
    makeStaticMap<std::string_view, void*>({{"test transmit serial", &test_transmit_serial_model},
                                            {"INS result transmit serial", &ins_result_transmit_serial_model},
                                            {"BMI088 heat PWM", &bmi088_heat_pwm_model},
                                            {"BMI088 communicate SPI", &bmi088_comm_spi_model},
                                            {"BMI088 SPI accel cs", &bmi088_accel_cs_gpio_model},
                                            {"BMI088 SPI gyro cs", &bmi088_gyro_cs_gpio_model}});

namespace timeline {

inline emdevif::atomic<uint32_t> overflow_count = 0;

extern "C" inline void TIM5_IRQHandler(void)
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

namespace detail {
inline constinit std::size_t timeLine = 0;
}

inline std::size_t getTimeLine() noexcept
{
    return detail::timeLine++;
}

namespace detail {
inline char buffer[512];
inline std::size_t buffer_head = 0;
}  // namespace detail

inline char* getBuffer() noexcept
{
    return detail::buffer;
}

inline ErrorCode printLogMessage(const char* message) noexcept
{
    const auto ret = ::snprintf(detail::buffer + detail::buffer_head,
                                std::size(detail::buffer) - detail::buffer_head,
                                "%s",
                                message);
    if (ret < 0) {
        EMDEVIF_FATAL_HANDLER("Failed to print log message");
    }

    detail::buffer_head += ret;

    return ErrorCode::Success;
}

}  // namespace logger

}  // namespace emdevif::user_declares
