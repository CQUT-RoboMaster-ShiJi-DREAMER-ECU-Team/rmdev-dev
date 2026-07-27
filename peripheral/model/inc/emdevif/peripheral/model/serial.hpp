/**
 * @file serial.hpp
 * @brief 串口外设模型定义
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_MODEL_SERIAL_HPP
#define EMDEVIF_PERIPHERAL_MODEL_SERIAL_HPP

#include <cstdint>

#include <span>
#include <type_traits>

#include "emdevif/core/error_handler.hpp"

namespace emdevif {

/// @brief 串口通信模型抽象类，定义串口外设的数据结构与函数接口类型
class SerialModel
{
public:
    /// @brief 串口状态枚举：就绪或忙碌
    enum State : uint_fast8_t {
        Ready = 0,
        Busy
    };

    /// @brief 串口状态获取函数类型，返回串口当前状态
    using GetStateFunction = State (*)(bool in_isr, void* handle);

    /// @brief 串口接收函数类型，阻塞式接收数据
    using ReceiveFunction = ErrorCode (*)(bool in_isr,
                                          void* handle,
                                          std::span<uint8_t> received_data,
                                          uint32_t timeout_ms);

    /// @brief 串口接收回调函数类型，接收完成时调用
    using ReceiveCallback = ErrorCode (*)(bool in_isr, void* handle, std::span<uint8_t> received_data);

    /// @brief 串口发送函数类型，发送数据
    using TransmitFunction = ErrorCode (*)(bool in_isr,
                                           void* handle,
                                           std::span<const uint8_t> data,
                                           uint32_t timeout_ms);

    /// @brief 串口实例结构体，聚合外设句柄与操作函数指针
    struct Instance {
        void* const handle{nullptr};                         ///< 外设句柄指针
        const GetStateFunction get_state_function{nullptr};  ///< 状态获取函数指针
        const ReceiveFunction receive_function{nullptr};     ///< 接收函数指针
        const ReceiveCallback receive_callback{nullptr};     ///< 接收回调函数指针
        const TransmitFunction transmit_function{nullptr};   ///< 发送函数指针
    };
};

static_assert(std::is_standard_layout_v<SerialModel::Instance>);

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_MODEL_SERIAL_HPP
