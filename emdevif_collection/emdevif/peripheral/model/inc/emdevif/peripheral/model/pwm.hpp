/**
 * @file pwm.hpp
 * @brief PWM 外设模型定义
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_MODEL_PWM_HPP
#define EMDEVIF_PERIPHERAL_MODEL_PWM_HPP

#include <cstdint>

#include <type_traits>

namespace emdevif {

/// @brief PWM 模型抽象类，定义 PWM 外设的数据结构与函数接口类型
class PwmModel
{
public:
    /// @brief PWM 外设启用函数类型
    using EnableFunction = void (*)(void* handle);
    /// @brief PWM 外设禁用函数类型
    using DisableFunction = void (*)(void* handle);

    /// @brief PWM 占空比设置函数类型（百分比范围 0~100）
    using setRatioFunction = void (*)(void* handle, uint8_t ratio);

    /// @brief PWM 实例结构体，聚合外设句柄与操作函数指针
    struct Instance {
        void* const handle{nullptr};               ///< 外设句柄指针
        const EnableFunction enable{nullptr};      ///< 启用函数指针
        const DisableFunction disable{nullptr};    ///< 禁用函数指针
        const setRatioFunction setRatio{nullptr};  ///< 占空比设置函数指针
    };
};

static_assert(std::is_standard_layout_v<PwmModel::Instance>);

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_MODEL_PWM_HPP
