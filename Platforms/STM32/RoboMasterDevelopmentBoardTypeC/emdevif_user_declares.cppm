/**
 * @file emdevif_user_declares.cppm
 * @author DuYicheng
 * @date 2025-09-04
 * @brief emdevif 由用户实现的功能
 */

module;

#include <string_view>
#include <limits>
#include <tuple>

#include "emdevif/fatal_handler.h"

#include "printf.h"

#include "usart.h"
#include "tim.h"
#include "spi.h"
#include "gpio.h"

export module emdevif.userDeclares;

import emdevif.container.map;
import emdevif.errorHandler;

import emdevif.sys.atomic;

import emdevif.peripheral.model.gpio;
import emdevif.peripheral.model.pwm;
import emdevif.peripheral.model.serial;
import emdevif.peripheral.model.spi;

import emdevif.stm32Peripheral.hal.pwm;
import emdevif.stm32Peripheral.hal.gpio;
import emdevif.stm32Peripheral.hal.usart;
import emdevif.stm32Peripheral.hal.spi;

export namespace emdevif::user_declares {

constinit emdevif::SerialModel::Instance test_transmit_serial_model{
    .handle_ = &huart6,
    .transmit_function_ = emdevif::stm32hal::uartTransmitBlocking};

constinit emdevif::SerialModel::Instance ins_result_transmit_serial_model{
    .handle_ = &huart1,
    .transmit_function_ = emdevif::stm32hal::uartTransmitBlocking};

constinit emdevif::stm32hal::PwmHandle bmi088_heat_pwm_handle{&htim10, TIM_CHANNEL_1};
constinit emdevif::PwmModel::Instance bmi088_heat_pwm_model{.handle_ = &bmi088_heat_pwm_handle,
                                                            .enable_ = emdevif::stm32hal::pwmEnable,
                                                            .disable_ = emdevif::stm32hal::pwmDisable,
                                                            .setRatio_ = emdevif::stm32hal::pwmSetRatio};

constinit emdevif::SpiModel::Instance bmi088_comm_spi_model{
    .handle_ = &hspi1,
    .transmit_receive_function_ = emdevif::stm32hal::spiTransmitReceiveBlock};

emdevif::stm32hal::GpioHandle bmi088_accel_cs_gpio{CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin};
constinit emdevif::GpioModel::Instance bmi088_accel_cs_gpio_model{.handle_ = &bmi088_accel_cs_gpio,
                                                                  .write_function_ = stm32hal::gpioWrite};

emdevif::stm32hal::GpioHandle bmi088_gyro_cs_gpio{CS1_GYRO_GPIO_Port, CS1_GYRO_Pin};
constinit emdevif::GpioModel::Instance bmi088_gyro_cs_gpio_model{.handle_ = &bmi088_gyro_cs_gpio,
                                                                 .write_function_ = stm32hal::gpioWrite};

constexpr auto peripheral_handle_map =
    makeStaticMap<std::string_view, void*>({{"test transmit serial", &test_transmit_serial_model},
                                            {"INS result transmit serial", &ins_result_transmit_serial_model},
                                            {"BMI088 heat PWM", &bmi088_heat_pwm_model},
                                            {"BMI088 communicate SPI", &bmi088_comm_spi_model},
                                            {"BMI088 SPI accel cs", &bmi088_accel_cs_gpio_model},
                                            {"BMI088 SPI gyro cs", &bmi088_gyro_cs_gpio_model}});

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
