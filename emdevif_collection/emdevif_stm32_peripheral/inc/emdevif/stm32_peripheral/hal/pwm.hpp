/**
 * @file pwm.hpp
 * @brief STM32 HAL PWM 外设适配，封装 HAL PWM 启停及占空比设置函数供 emdevif::Pwm 使用
 */

#pragma once
#ifndef EMDEVIF_STM32_PERIPHERAL_HAL_PWM_HPP
#define EMDEVIF_STM32_PERIPHERAL_HAL_PWM_HPP

#include <cstdint>

#include "emdevif/core/fatal_handler.h"
#include "tim.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/stm32_peripheral/hal/detail/hal_status_mapper.hpp"

namespace emdevif::stm32hal {

/**
 * @brief PWM 句柄，封装 STM32 定时器句柄和通道号
 */
struct PwmHandle {
    TIM_HandleTypeDef* htim;  ///< 定时器句柄
    uint32_t channel;         ///< PWM 通道号
};

namespace detail {

/**
 * @brief 获取 PWM 定时器的计数方向模式
 * @param[in] pwm_handle PWM 句柄指针
 * @return 计数方向模式，TIM_COUNTERMODE_UP 或 TIM_COUNTERMODE_DOWN
 */
inline uint32_t pwmGetCounterMode(const PwmHandle* pwm_handle) noexcept
{
    return (READ_BIT(pwm_handle->htim->Instance->CR1, TIM_CR1_DIR) == 0 ? TIM_COUNTERMODE_UP : TIM_COUNTERMODE_DOWN);
}

/**
 * @brief 获取 PWM 通道的输出比较模式
 * @param[in] pwm_handle PWM 句柄指针
 * @return PWM 输出比较模式（如 TIM_OCMODE_PWM1 或 TIM_OCMODE_PWM2）
 */
inline uint32_t pwmGetPwmMode(const PwmHandle* pwm_handle) noexcept
{
    const auto [htim, channel] = *pwm_handle;

    uint32_t mode;

    switch (channel) {
    case TIM_CHANNEL_1:
        mode = READ_BIT(htim->Instance->CCMR1, TIM_CCMR1_OC1M);
        break;
    case TIM_CHANNEL_2:
        mode = READ_BIT(htim->Instance->CCMR1, TIM_CCMR1_OC2M);
        break;
    case TIM_CHANNEL_3:
        mode = READ_BIT(htim->Instance->CCMR2, TIM_CCMR2_OC3M);
        break;
    case TIM_CHANNEL_4:
        mode = READ_BIT(htim->Instance->CCMR2, TIM_CCMR2_OC4M);
        break;
    default:
        EMDEVIF_FATAL_HANDLER("Invalid arguments!");
    }

    return mode;
}

}  // namespace detail

/**
 * @brief 使能 PWM 输出
 * @param[in] handle 指向 PwmHandle 的指针
 */
inline void pwmEnable(void* handle) noexcept
{
    const auto pwm_handle = static_cast<PwmHandle*>(handle);

    HAL_TIM_PWM_Start(pwm_handle->htim, pwm_handle->channel);
}

/**
 * @brief 禁止 PWM 输出
 * @param[in] handle 指向 PwmHandle 的指针
 */
inline void pwmDisable(void* handle) noexcept
{
    const auto pwm_handle = static_cast<PwmHandle*>(handle);

    HAL_TIM_PWM_Stop(pwm_handle->htim, pwm_handle->channel);
}

/**
 * @brief 设置 PWM 占空比
 *
 * 根据 PWM 模式和计数方向自动调整比较值，确保占空比语义正确。
 * @param[in] handle 指向 PwmHandle 的指针
 * @param ratio 占空比，取值范围 0~100（对应 0%~100%），精度为 1%
 */
inline void pwmSetRatio(void* handle, const uint8_t ratio) noexcept
{
    const uint32_t real_ratio = ratio * 100;
    auto* const pwm_handle = static_cast<PwmHandle*>(handle);
    const auto [htim, channel] = *pwm_handle;
    const uint32_t autoreload_value = __HAL_TIM_GET_AUTORELOAD(pwm_handle->htim);

    uint32_t real_compare_value = real_ratio * autoreload_value / 10000U;

    const auto pwm_mode = detail::pwmGetPwmMode(pwm_handle);
    const auto pwm_counter_mode = detail::pwmGetCounterMode(pwm_handle);
    if (pwm_mode == TIM_OCMODE_PWM1) {
        if (pwm_counter_mode == TIM_COUNTERMODE_UP) {
            // do nothing
        }
        else {
            real_compare_value = autoreload_value - real_compare_value;
        }
    }
    else if (pwm_mode == TIM_OCMODE_PWM2) {
        if (pwm_counter_mode == TIM_COUNTERMODE_UP) {
            real_compare_value = autoreload_value - real_compare_value;
        }
        else {
            // do nothing
        }
    }

    if (real_compare_value > autoreload_value) {
        real_compare_value = autoreload_value;
    }

    __HAL_TIM_SET_COMPARE(htim, channel, real_compare_value);
}

}  // namespace emdevif::stm32hal

#endif  // !EMDEVIF_STM32_PERIPHERAL_HAL_PWM_HPP
