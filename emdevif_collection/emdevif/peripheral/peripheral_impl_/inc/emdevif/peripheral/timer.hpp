/**
 * @file timer.hpp
 * @brief 定时器外设封装类
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_TIMER_HPP
#define EMDEVIF_PERIPHERAL_TIMER_HPP

#include <cstdint>

#include <string_view>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/peripheral/detail/peripheral_error_handler.hpp"
#include "emdevif/peripheral/model/timer.hpp"
#include "emdevif/peripheral/peripheral_handle_map.hpp"
namespace emdevif {

/// @brief 定时器外设封装类，通过句柄映射访问底层定时器外设并封装定时操作
class Timer : TimerModel
{
private:
    TimerModel::Instance* instance_;

public:
    /// @brief 根据句柄名构造定时器实例
    /// @param name 定时器外设句柄名称
    explicit Timer(const std::string_view name) noexcept
        : instance_(static_cast<TimerModel::Instance*>(PeripheralHandleMap::findHandle(name)))
    {
        detail::PeripheralErrorHandler::checkInstanceIsExist(instance_, "Timer", name);
    }

    /// @brief 启用定时器
    void enable() const noexcept
    {
        EMDEVIF_ASSERT(instance_->enable != nullptr);
        instance_->enable(instance_->handle);
    }

    /// @brief 禁用定时器
    void disable() const noexcept
    {
        EMDEVIF_ASSERT(instance_->disable != nullptr);
        instance_->disable(instance_->handle);
    }

    /// @brief 获取定时器当前计数值
    /// @return 当前计数值
    [[nodiscard]] uint32_t getCounter() const noexcept
    {
        EMDEVIF_ASSERT(instance_->getCounter != nullptr);
        return instance_->getCounter(instance_->handle);
    }

    /// @brief 设置定时器计数值
    /// @param counter 目标计数值
    void setCounter(const uint32_t counter) const noexcept
    {
        EMDEVIF_ASSERT(instance_->setCounter != nullptr);
        instance_->setCounter(instance_->handle, counter);
    }

    /// @brief 启用定时器中断
    void enableInterrupt() const noexcept
    {
        EMDEVIF_ASSERT(instance_->enableInterrupt != nullptr);
        instance_->enableInterrupt(instance_->handle);
    }
    /// @brief 禁用定时器中断
    void disableInterrupt() const noexcept
    {
        EMDEVIF_ASSERT(instance_->disableInterrupt != nullptr);
        instance_->disableInterrupt(instance_->handle);
    }

    /// @brief 定时器周期到达回调入口
    /// @param handle 外设句柄
    void periodElapsedCallback(void* handle) const noexcept
    {
        EMDEVIF_ASSERT(instance_->periodElapsed != nullptr);
        instance_->periodElapsed(handle);
    }
};

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_TIMER_HPP
