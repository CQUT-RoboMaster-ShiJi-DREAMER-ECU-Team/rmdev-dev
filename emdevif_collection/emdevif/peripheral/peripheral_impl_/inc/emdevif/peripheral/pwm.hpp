/**
 * @file pwm.hpp
 * @brief PWM 外设封装类
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_PWM_HPP
#define EMDEVIF_PERIPHERAL_PWM_HPP

#include <cstdint>

#include <span>
#include <string_view>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/peripheral/detail/peripheral_error_handler.hpp"
#include "emdevif/peripheral/model/pwm.hpp"
#include "emdevif/peripheral/peripheral_handle_map.hpp"
namespace emdevif {

/// @brief PWM 外设封装类，通过句柄映射访问底层 PWM 外设并封装启停及占空比设置操作
class Pwm : public PwmModel
{
private:
    PwmModel::Instance* instance_;

public:
    /// @brief 根据句柄名构造 PWM 实例
    /// @param name PWM 外设句柄名称
    explicit Pwm(const std::string_view name) noexcept
        : instance_(static_cast<PwmModel::Instance*>(PeripheralHandleMap::findHandle(name)))
    {
        detail::PeripheralErrorHandler::checkInstanceIsExist(instance_, "PWM", name);
    }

    /// @brief 启用 PWM 输出
    void enable() const noexcept
    {
        instance_->enable(instance_->handle);
    }

    /// @brief 禁用 PWM 输出
    void disable() const noexcept
    {
        instance_->disable(instance_->handle);
    }

    /// @brief 设置 PWM 占空比
    /// @param ratio 占空比百分值，范围 0~100
    void setRatio(const uint8_t ratio) const noexcept
    {
        instance_->setRatio(instance_->handle, ratio);
    }
};

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_PWM_HPP
