/**
 * @file serial.hpp
 * @brief 串口通信外设封装类
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_SERIAL_HPP
#define EMDEVIF_PERIPHERAL_SERIAL_HPP

#include <cstdint>

#include <limits>
#include <span>
#include <string_view>
#include <type_traits>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/peripheral/detail/peripheral_error_handler.hpp"
#include "emdevif/peripheral/model/serial.hpp"
#include "emdevif/peripheral/peripheral_handle_map.hpp"
namespace emdevif {

/// @brief 串口通信外设封装类，通过句柄映射访问底层串口外设并封装收发操作
class Serial : public SerialModel
{
private:
    SerialModel::Instance* instance_;

public:
    Serial() = delete;

    /// @brief 根据句柄名构造串口实例
    /// @param name 串口外设句柄名称
    explicit Serial(const std::string_view name) noexcept
        : instance_(static_cast<SerialModel::Instance*>(PeripheralHandleMap::findHandle(name)))
    {
        detail::PeripheralErrorHandler::checkInstanceIsExist(instance_, "Serial", name);
    }

    /// @brief 获取串口当前状态
    /// @param in_isr 是否在中断上下文中调用
    /// @return 串口当前状态
    SerialModel::State getStatus(const bool in_isr) const noexcept  // NOLINT(*-use-nodiscard)
    {
        EMDEVIF_ASSERT(instance_->get_state_function != nullptr);
        return instance_->get_state_function(in_isr, instance_->handle);
    }

    /// @brief 串口接收数据
    /// @param in_isr 是否在中断上下文中调用
    /// @param received_data [out] 接收数据缓冲区
    /// @param timeout_ms 超时时间（毫秒）
    /// @return 错误码
    ErrorCode receive(const bool in_isr,  // NOLINT(*-use-nodiscard)
                      const std::span<uint8_t> received_data,
                      const uint32_t timeout_ms) const noexcept
    {
        EMDEVIF_ASSERT(instance_->receive_function != nullptr);
        return instance_->receive_function(in_isr, instance_->handle, received_data, timeout_ms);
    }

    /// @brief 串口接收回调入口，将接收到的数据转发给注册的回调函数
    /// @param in_isr 是否在中断上下文中调用
    /// @param handle 外设句柄
    /// @param received_data [out] 接收到的数据
    /// @return 错误码
    ErrorCode receiveCallbackEntry(const bool in_isr,
                                   void* handle,
                                   const std::span<uint8_t> received_data) const noexcept
    {
        EMDEVIF_ASSERT(instance_->receive_callback != nullptr);
        return instance_->receive_callback(in_isr, handle, received_data);
    }

    /// @brief 串口发送数据
    /// @param in_isr 是否在中断上下文中调用
    /// @param transmit_data 待发送数据
    /// @param timeout_ms 超时时间（毫秒）
    /// @return 错误码
    ErrorCode transmit(const bool in_isr,  // NOLINT(*-use-nodiscard)
                       const std::span<const uint8_t> transmit_data,
                       const uint32_t timeout_ms) const noexcept
    {
        EMDEVIF_ASSERT(instance_->transmit_function != nullptr);
        const auto status = instance_->transmit_function(in_isr, instance_->handle, transmit_data, timeout_ms);
        return status;
    }

    /// @brief 最大超时值，表示无限等待
    static constexpr uint32_t max_delay = std::numeric_limits<uint32_t>::max();
};

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_SERIAL_HPP
