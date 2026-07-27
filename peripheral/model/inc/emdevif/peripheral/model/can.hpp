/**
 * @file can.hpp
 * @brief CAN 外设模型定义
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_MODEL_CAN_HPP
#define EMDEVIF_PERIPHERAL_MODEL_CAN_HPP

#include <cstdint>

#include <span>
#include <type_traits>

#include "emdevif/core/error_handler.hpp"

namespace emdevif {

/// @brief CAN 通信模型抽象类，定义 CAN 外设的数据结构与函数接口类型
class CanModel
{
public:
    /// @brief CAN 报文标识符类型（IDE）：标准帧（11 位 ID）或扩展帧（29 位 ID）
    enum class DataIdentifier : uint_fast8_t {
        StandardId = 0,
        ExtendedId
    };

    /// @brief CAN 远程传输请求类型（RTR）：数据帧或远程帧
    enum class RemoteTransmissionRequest : uint_fast8_t {
        DataFrame = 0,
        RemoteFrame
    };

    /// @brief CAN 数据长度类型（DLC，最大支持 8 字节）
    using DataLength = uint_fast8_t;

    /// @brief CAN 报文帧头结构体，包含标识符、帧类型与数据长度码信息
    struct DataHeader {
        uint32_t id;                    ///< CAN 报文标识符（标准帧使用低 11 位，扩展帧使用低 29 位）
        DataIdentifier ide;             ///< 标识符类型：标准帧（11 位 ID）或扩展帧（29 位 ID）
        RemoteTransmissionRequest rtr;  ///< 帧类型：数据帧或远程帧
        DataLength dlc;                 ///< 数据长度码（单位：字节）
    };

    /// @brief CAN 接收回调函数类型，用于在接收到 CAN 报文时调用
    using ReceiveCallback = ErrorCode (*)(bool in_isr,
                                          void* handle,
                                          DataHeader& received_header,
                                          std::span<uint8_t> received_data);

    /// @brief CAN 发送函数类型，用于发送 CAN 报文
    using TransmitFunction = ErrorCode (*)(bool in_isr,
                                           void* handle,
                                           const DataHeader& header,
                                           std::span<const uint8_t> data);

    /// @brief CAN 实例结构体，聚合外设句柄与操作函数指针
    struct Instance {
        void* const handle{nullptr};                        ///< 外设句柄指针
        const ReceiveCallback receive_callback{nullptr};    ///< 接收回调函数指针
        const TransmitFunction transmit_function{nullptr};  ///< 发送函数指针
    };
};

static_assert(std::is_standard_layout_v<CanModel::Instance>);

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_MODEL_CAN_HPP
