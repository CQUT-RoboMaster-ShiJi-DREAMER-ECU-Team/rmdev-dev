/**
 * @file gpio.hpp
 * @brief GPIO 外设封装类
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_GPIO_HPP
#define EMDEVIF_PERIPHERAL_GPIO_HPP

#include <cstdint>

#include <span>
#include <string_view>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/peripheral/detail/peripheral_error_handler.hpp"
#include "emdevif/peripheral/model/gpio.hpp"
#include "emdevif/peripheral/peripheral_handle_map.hpp"

namespace emdevif {

/// @brief GPIO 外设封装类，通过句柄映射访问底层 GPIO 外设并封装引脚读写翻转操作
class Gpio : public GpioModel
{
private:
    GpioModel::Instance* const instance_;

public:
    /**
     * @brief 根据句柄名构造 GPIO 实例
     * @param name GPIO 外设句柄名称
     */
    explicit Gpio(const std::string_view name) noexcept
        : instance_(static_cast<GpioModel::Instance*>(PeripheralHandleMap::findHandle(name)))
    {
        detail::PeripheralErrorHandler::checkInstanceIsExist(instance_, "GPIO", name);
    }

    /**
     * @brief 写入 GPIO 引脚状态
     * @param pin_state 目标引脚状态（置位或复位）
     */
    void write(const PinState pin_state) const noexcept
    {
        EMDEVIF_ASSERT(instance_->write_function != nullptr);
        instance_->write_function(instance_->handle, static_cast<uint_fast8_t>(pin_state));
    }

    /**
     * @brief 读取 GPIO 引脚当前状态
     * @return 当前引脚状态
     */
    [[nodiscard]] PinState read() const noexcept
    {
        EMDEVIF_ASSERT(instance_->write_function != nullptr);
        return (instance_->read_function(instance_->handle) == 0U ? Gpio::Reset : Gpio::Set);
    }

    /**
     * @brief 翻转 GPIO 引脚状态
     */
    void toggle() const noexcept
    {
        EMDEVIF_ASSERT(instance_->write_function != nullptr);
        instance_->toggle_function(instance_->handle);
    }
};

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_GPIO_HPP
