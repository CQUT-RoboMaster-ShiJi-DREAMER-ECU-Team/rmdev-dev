/**
 * @file spi.hpp
 * @brief STM32 HAL SPI 外设适配，封装 HAL SPI 收发函数供 emdevif::Spi 使用
 */

#pragma once
#ifndef EMDEVIF_STM32_PERIPHERAL_HAL_SPI_HPP
#define EMDEVIF_STM32_PERIPHERAL_HAL_SPI_HPP

#include <cstdint>

#include <span>

#include "emdevif/core/attributes_and_useful_macros.h"
#include "emdevif/core/fatal_handler.h"
#include "spi.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/stm32_peripheral/hal/detail/hal_status_mapper.hpp"

namespace emdevif::stm32hal {

/**
 * @brief SPI 阻塞式全双工收发
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle SPI 外设句柄，指向 HAL SPI_HandleTypeDef
 * @param tx_data 发送数据缓冲区
 * @param[out] rx_data 接收数据缓冲区，长度须与 tx_data 一致
 * @param timeout_ms 超时时间（毫秒）
 * @return 操作结果错误码
 */
inline ErrorCode spiTransmitReceiveBlock(const bool in_isr,
                                         void* handle,
                                         const std::span<const uint8_t> tx_data,
                                         const std::span<uint8_t> rx_data,
                                         const uint32_t timeout_ms) noexcept
{
    EMDEVIF_ASSERT(tx_data.size_bytes() == rx_data.size_bytes());

    EMDEVIF_UNUSED(in_isr);

    const auto status = HAL_SPI_TransmitReceive(static_cast<SPI_HandleTypeDef*>(handle),
                                                tx_data.data(),
                                                rx_data.data(),
                                                tx_data.size_bytes(),
                                                timeout_ms);
    return detail::halStatusToErrorCode(status);
}

/**
 * @brief SPI 中断模式全双工收发
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle SPI 外设句柄，指向 HAL SPI_HandleTypeDef
 * @param tx_data 发送数据缓冲区
 * @param[out] rx_data 接收数据缓冲区，长度须与 tx_data 一致
 * @param timeout_ms 超时时间（当前未使用）
 * @return 操作结果错误码
 */
inline ErrorCode spiTransmitReceiveInterrupt(const bool in_isr,
                                             void* handle,
                                             const std::span<const uint8_t> tx_data,
                                             const std::span<uint8_t> rx_data,
                                             const uint32_t timeout_ms) noexcept
{
    EMDEVIF_ASSERT(tx_data.size_bytes() == rx_data.size_bytes());

    EMDEVIF_UNUSED(in_isr);
    EMDEVIF_UNUSED(timeout_ms);

    const auto status = HAL_SPI_TransmitReceive_IT(static_cast<SPI_HandleTypeDef*>(handle),
                                                   tx_data.data(),
                                                   rx_data.data(),
                                                   tx_data.size_bytes());
    return detail::halStatusToErrorCode(status);
}

/**
 * @brief SPI DMA 模式全双工收发
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle SPI 外设句柄，指向 HAL SPI_HandleTypeDef
 * @param tx_data 发送数据缓冲区
 * @param[out] rx_data 接收数据缓冲区，长度须与 tx_data 一致
 * @param timeout_ms 超时时间（当前未使用）
 * @return 操作结果错误码
 */
inline ErrorCode spiTransmitReceiveDma(const bool in_isr,
                                       void* handle,
                                       const std::span<const uint8_t> tx_data,
                                       const std::span<uint8_t> rx_data,
                                       const uint32_t timeout_ms) noexcept
{
    EMDEVIF_ASSERT(tx_data.size_bytes() == rx_data.size_bytes());

    EMDEVIF_UNUSED(in_isr);
    EMDEVIF_UNUSED(timeout_ms);

    const auto status = HAL_SPI_TransmitReceive_DMA(static_cast<SPI_HandleTypeDef*>(handle),
                                                    tx_data.data(),
                                                    rx_data.data(),
                                                    tx_data.size_bytes());
    return detail::halStatusToErrorCode(status);
}

}  // namespace emdevif::stm32hal

#endif  // !EMDEVIF_STM32_PERIPHERAL_HAL_SPI_HPP
