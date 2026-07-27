/**
 * @file pwm_hal_impl.cppm
 * @brief STM32 HAL 库的 PWM 函数的封装，用以 emdevif::Pwm 类的回调函数
 */

module;

#include "tim.h"
#include "emdevif/stm32_peripheral/hal/pwm.hpp"

export module emdevif.stm32_peripheral.hal.pwm;

export namespace emdevif::stm32hal {
using ::emdevif::stm32hal::pwmDisable;
using ::emdevif::stm32hal::pwmEnable;
using ::emdevif::stm32hal::PwmHandle;
using ::emdevif::stm32hal::pwmSetRatio;
}  // namespace emdevif::stm32hal
