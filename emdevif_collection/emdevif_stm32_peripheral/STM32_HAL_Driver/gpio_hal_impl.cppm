/**
 * @file gpio_hal_impl.cppm
 * @brief STM32 HAL 库的 GPIO 函数的封装，用以 emdevif::Gpio 类的回调函数
 */

module;

#include "gpio.h"
#include "emdevif/stm32_peripheral/hal/gpio.hpp"

export module emdevif.stm32_peripheral.hal.gpio;

export namespace emdevif::stm32hal {
using ::emdevif::stm32hal::GpioHandle;
using ::emdevif::stm32hal::gpioRead;
using ::emdevif::stm32hal::gpioToggle;
using ::emdevif::stm32hal::gpioWrite;
}  // namespace emdevif::stm32hal
