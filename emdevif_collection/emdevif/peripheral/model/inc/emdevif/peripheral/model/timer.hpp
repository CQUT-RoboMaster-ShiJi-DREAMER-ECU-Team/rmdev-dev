/**
 * @file timer.hpp
 * @brief 定时器外设模型定义
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_MODEL_TIMER_HPP
#define EMDEVIF_PERIPHERAL_MODEL_TIMER_HPP

#include <cstdint>

#include <span>
#include <type_traits>

namespace emdevif {

/// @brief 定时器模型抽象类，定义定时器外设的数据结构与函数接口类型
class TimerModel
{
public:
    /// @brief 定时器启用函数类型
    using EnableFunction = void (*)(void* handle);
    /// @brief 定时器禁用函数类型
    using DisableFunction = void (*)(void* handle);

    /// @brief 定时器计数值获取函数类型
    using GetCounterFunction = uint32_t (*)(void* handle);
    /// @brief 定时器计数值设置函数类型
    using SetCounterFunction = void (*)(void* handle, uint32_t counter);

    /// @brief 定时器中断启用函数类型
    using EnableInterruptFunction = void (*)(void* handle);
    /// @brief 定时器中断禁用函数类型
    using DisableInterruptFunction = void (*)(void* handle);

    /// @brief 定时器周期到达回调函数类型
    using PeriodElapsedCallback = void (*)(void* handle);

    /// @brief 定时器实例结构体，聚合外设句柄与操作函数指针
    struct Instance {
        void* const handle{nullptr};                               ///< 外设句柄指针
        const EnableFunction enable{nullptr};                      ///< 启用函数指针
        const DisableFunction disable{nullptr};                    ///< 禁用函数指针
        const GetCounterFunction getCounter{nullptr};              ///< 计数值获取函数指针
        const SetCounterFunction setCounter{nullptr};              ///< 计数值设置函数指针
        const EnableInterruptFunction enableInterrupt{nullptr};    ///< 中断启用函数指针
        const DisableInterruptFunction disableInterrupt{nullptr};  ///< 中断禁用函数指针
        const PeriodElapsedCallback periodElapsed{nullptr};        ///< 周期到达回调函数指针
    };
};

static_assert(std::is_standard_layout_v<TimerModel::Instance>);

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_MODEL_TIMER_HPP
