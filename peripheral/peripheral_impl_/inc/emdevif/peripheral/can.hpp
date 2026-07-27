/**
 * @file can.hpp
 * @brief CAN 通信外设封装类，通过句柄映射访问底层 CAN 外设并封装收发操作
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_CAN_HPP
#define EMDEVIF_PERIPHERAL_CAN_HPP

#include <cstdint>

#include <span>
#include <string_view>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/peripheral/detail/peripheral_error_handler.hpp"
#include "emdevif/peripheral/model/can.hpp"
#include "emdevif/peripheral/peripheral_handle_map.hpp"
namespace emdevif {

/// @brief CAN 通信外设封装类：通过句柄映射访问底层 CAN 外设并封装收发操作
class Can : public CanModel
{
private:
    CanModel::Instance* const instance_;

public:
    Can() = delete;

    /**
     * @brief 根据句柄名构造 CAN 实例
     * @param name CAN 外设句柄名称
     */
    explicit Can(const std::string_view name) noexcept
        : instance_(static_cast<CanModel::Instance*>(PeripheralHandleMap::findHandle(name)))
    {
        detail::PeripheralErrorHandler::checkInstanceIsExist(instance_, "CAN", name);
    }

    /**
     * @brief CAN 接收回调入口，将接收到的报文转发给注册的回调函数
     * @param in_isr 是否在中断上下文中调用
     * @param handle 外设句柄
     * @param[out] received_header 接收到的报文帧头
     * @param[out] received_data 接收到的数据缓冲区
     * @return 错误码
     */
    ErrorCode receiveCallbackEntry(const bool in_isr,
                                   void* handle,
                                   DataHeader& received_header,
                                   const std::span<uint8_t> received_data) const noexcept
    {
        EMDEVIF_ASSERT(instance_->receive_callback != nullptr);
        return instance_->receive_callback(in_isr, handle, received_header, received_data);
    }

    /**
     * @brief 发送 CAN 报文
     * @param in_isr 是否在中断上下文中调用
     * @param header 待发送报文帧头
     * @param data 待发送数据
     * @return 错误码
     */
    ErrorCode transmit(const bool in_isr, const DataHeader& header, const std::span<const uint8_t> data) const noexcept
    {
        EMDEVIF_ASSERT(instance_->transmit_function != nullptr);
        return instance_->transmit_function(in_isr, instance_->handle, header, data);
    }
};

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_CAN_HPP
