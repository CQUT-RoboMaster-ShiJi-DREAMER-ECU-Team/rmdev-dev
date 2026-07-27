/**
 * @file spi.hpp
 * @brief SPI 通信外设模型定义
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_MODEL_SPI_HPP
#define EMDEVIF_PERIPHERAL_MODEL_SPI_HPP

#include <cstdint>

#include <span>
#include <type_traits>

#include "emdevif/core/error_handler.hpp"

namespace emdevif {

/// @brief SPI 通信模型抽象类，定义 SPI 外设的数据结构与函数接口类型
class SpiModel
{
public:
    /// @brief SPI 全双工收发函数类型
    using TransmitReceiveFunction = ErrorCode (*)(bool in_isr,
                                                  void* handle,
                                                  std::span<const uint8_t> tx_data,
                                                  std::span<uint8_t> rx_data,
                                                  uint32_t timeout_ms);

    /// @brief SPI 实例结构体，聚合外设句柄与操作函数指针
    struct Instance {
        void* const handle{nullptr};                                       ///< 外设句柄指针
        const TransmitReceiveFunction transmit_receive_function{nullptr};  ///< 全双工收发函数指针
    };
};

static_assert(std::is_standard_layout_v<SpiModel::Instance>);

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_MODEL_SPI_HPP
