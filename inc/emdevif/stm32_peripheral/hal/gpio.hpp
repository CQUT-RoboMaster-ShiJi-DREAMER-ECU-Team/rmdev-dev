/**
 * @file gpio.hpp
 * @brief STM32 HAL GPIO 外设适配，封装 HAL GPIO 读写翻转函数供 emdevif::Gpio 使用
 */

#pragma once
#ifndef EMDEVIF_STM32_PERIPHERAL_HAL_GPIO_HPP
#define EMDEVIF_STM32_PERIPHERAL_HAL_GPIO_HPP

#include <cstdint>

#include "gpio.h"

#include "emdevif/core/error_handler.hpp"

namespace emdevif::stm32hal {

/**
 * @brief GPIO 句柄，封装 STM32 GPIO 端口和引脚号
 */
struct GpioHandle {
    GPIO_TypeDef* port;  ///< GPIO 端口基地址
    uint32_t pin;        ///< 引脚号
};

/**
 * @brief 设置 GPIO 输出电平
 * @param[in] handle 指向 GpioHandle 的指针
 * @param pin_state 引脚状态，非零为高电平，零为低电平
 */
inline void gpioWrite(void* handle, const uint_fast8_t pin_state) noexcept
{
    const auto gpio_handle = static_cast<GpioHandle*>(handle);
    HAL_GPIO_WritePin(gpio_handle->port, gpio_handle->pin, (pin_state == 0U ? GPIO_PIN_RESET : GPIO_PIN_SET));
}

/**
 * @brief 读取 GPIO 输入电平
 * @param[in] handle 指向 GpioHandle 的指针
 * @return 引脚状态，1 表示高电平，0 表示低电平
 */
inline uint_fast8_t gpioRead(void* handle) noexcept
{
    const auto gpio_handle = static_cast<GpioHandle*>(handle);
    return (HAL_GPIO_ReadPin(gpio_handle->port, gpio_handle->pin) == GPIO_PIN_RESET ? 0U : 1U);
}

/**
 * @brief 翻转 GPIO 输出电平
 * @param[in] handle 指向 GpioHandle 的指针
 */
inline void gpioToggle(void* handle) noexcept
{
    const auto gpio_handle = static_cast<GpioHandle*>(handle);
    HAL_GPIO_TogglePin(gpio_handle->port, gpio_handle->pin);
}

}  // namespace emdevif::stm32hal

#endif  // !EMDEVIF_STM32_PERIPHERAL_HAL_GPIO_HPP