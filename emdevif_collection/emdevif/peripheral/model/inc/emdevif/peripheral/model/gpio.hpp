/**
 * @file gpio.hpp
 * @brief GPIO 外设模型定义
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_MODEL_GPIO_HPP
#define EMDEVIF_PERIPHERAL_MODEL_GPIO_HPP

#include <cstdint>

#include <type_traits>

namespace emdevif {

/// @brief GPIO 模型抽象类：定义 GPIO 外设的数据结构与函数接口类型
class GpioModel
{
public:
    /// @brief GPIO 引脚状态枚举：复位（低电平）或置位（高电平）
    enum PinState : uint_fast8_t {
        Reset = 0,  ///< 复位（低电平）
        Set = 1     ///< 置位（高电平）
    };

    /// @brief GPIO 引脚写入函数类型
    using WriteFunction = void (*)(void* handle, uint_fast8_t pin_state);
    /// @brief GPIO 引脚读取函数类型
    using ReadFunction = uint_fast8_t (*)(void* handle);
    /// @brief GPIO 引脚翻转函数类型
    using ToggleFunction = void (*)(void* handle);

    /// @brief GPIO 实例结构体，聚合外设句柄与操作函数指针
    struct Instance {
        void* const handle{nullptr};                    ///< 外设句柄指针
        const WriteFunction write_function{nullptr};    ///< 写入引脚电平函数指针
        const ReadFunction read_function{nullptr};      ///< 读取引脚电平函数指针
        const ToggleFunction toggle_function{nullptr};  ///< 翻转引脚电平函数指针
    };
};

static_assert(std::is_standard_layout_v<GpioModel::Instance>);

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_MODEL_GPIO_HPP
