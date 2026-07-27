/**
 * @file spi.hpp
 * @brief SPI 通信外设封装类
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_SPI_HPP
#define EMDEVIF_PERIPHERAL_SPI_HPP

#include <cstdint>

#include <limits>
#include <span>
#include <string_view>
#include <type_traits>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/peripheral/detail/peripheral_error_handler.hpp"
#include "emdevif/peripheral/model/spi.hpp"
#include "emdevif/peripheral/peripheral_handle_map.hpp"
namespace emdevif {

/// @brief SPI 通信外设封装类，通过句柄映射访问底层 SPI 外设并封装全双工收发操作
class Spi : public SpiModel
{
private:
    SpiModel::Instance* instance_;

public:
    Spi() = delete;

    /// @brief 根据句柄名构造 SPI 实例
    /// @param name SPI 外设句柄名称
    explicit Spi(const std::string_view name) noexcept
        : instance_(static_cast<SpiModel::Instance*>(PeripheralHandleMap::findHandle(name)))
    {
        detail::PeripheralErrorHandler::checkInstanceIsExist(instance_, "SPI", name);
    }

    /// @brief SPI 全双工收发数据
    /// @param in_isr 是否在中断上下文中调用
    /// @param tx_data 待发送数据
    /// @param rx_data [out] 接收数据缓冲区
    /// @param timeout_ms 超时时间（毫秒）
    /// @return 错误码
    /// @retval ErrorCode::InvalidArgument tx_data 与 rx_data 大小不匹配
    ErrorCode transmitReceive(const bool in_isr,
                              const std::span<const uint8_t> tx_data,
                              const std::span<uint8_t> rx_data,
                              const uint32_t timeout_ms) const noexcept
    {
        if (tx_data.size() != rx_data.size()) {
            return ErrorCode::InvalidArgument;
        }

        return instance_->transmit_receive_function(in_isr, instance_->handle, tx_data, rx_data, timeout_ms);
    }

    /// @brief 最大超时值，表示无限等待
    static constexpr uint32_t max_delay = std::numeric_limits<uint32_t>::max();
};

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_SPI_HPP
