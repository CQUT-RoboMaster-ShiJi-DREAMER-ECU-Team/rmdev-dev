/**
 * @file usart.hpp
 * @brief STM32 HAL USART 外设适配，封装 HAL USART 收发函数供 emdevif::Serial 使用
 */

#pragma once
#ifndef EMDEVIF_STM32_PERIPHERAL_HAL_USART_HPP
#define EMDEVIF_STM32_PERIPHERAL_HAL_USART_HPP

#include <span>

#include "usart.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/peripheral/model/serial.hpp"
#include "emdevif/stm32_peripheral/hal/detail/hal_status_mapper.hpp"

namespace emdevif::stm32hal {

/// UART 最大阻塞等待延时值
constexpr uint32_t uart_max_delay = HAL_MAX_DELAY;

/// UART 非阻塞等待值（立即返回）
constexpr uint32_t uart_none_blocking = 0U;

/**
 * @brief 获取 USART 当前状态
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle USART 外设句柄，指向 HAL UART_HandleTypeDef
 * @return 串口状态
 * @retval SerialModel::Ready 就绪
 * @retval SerialModel::Busy 忙碌
 */
inline SerialModel::State uartGetState(bool, void* handle) noexcept
{
    if (HAL_UART_GetState(static_cast<UART_HandleTypeDef*>(handle)) == HAL_UART_STATE_READY) {
        return SerialModel::Ready;
    }

    return SerialModel::Busy;
}

/**
 * @brief UART 阻塞接收
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle USART 外设句柄，指向 HAL UART_HandleTypeDef
 * @param[out] received_data 接收数据缓冲区
 * @param timeout_ms 超时时间（毫秒）
 * @return 操作结果错误码
 */
inline ErrorCode uartReceiveBlocking(bool, void* handle, std::span<uint8_t> received_data, const uint32_t timeout_ms)
{
    const auto status = HAL_UART_Receive(static_cast<UART_HandleTypeDef*>(handle),
                                         received_data.data(),
                                         received_data.size_bytes(),
                                         timeout_ms);
    return detail::halStatusToErrorCode(status);
}

/**
 * @brief UART 中断模式接收
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle USART 外设句柄，指向 HAL UART_HandleTypeDef
 * @param[out] received_data 接收数据缓冲区
 * @param timeout_ms 超时时间（当前未使用）
 * @return 操作结果错误码
 */
inline ErrorCode uartReceiveInterrupt(bool, void* handle, std::span<uint8_t> received_data, uint32_t)
{
    const auto status =
        HAL_UART_Receive_IT(static_cast<UART_HandleTypeDef*>(handle), received_data.data(), received_data.size_bytes());
    return detail::halStatusToErrorCode(status);
}

/**
 * @brief UART DMA 模式接收
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle USART 外设句柄，指向 HAL UART_HandleTypeDef
 * @param[out] received_data 接收数据缓冲区
 * @param timeout_ms 超时时间（当前未使用）
 * @return 操作结果错误码
 */
inline ErrorCode uartReceiveDma(bool, void* handle, std::span<uint8_t> received_data, uint32_t)
{
    const auto status = HAL_UART_Receive_DMA(static_cast<UART_HandleTypeDef*>(handle),
                                             received_data.data(),
                                             received_data.size_bytes());
    return detail::halStatusToErrorCode(status);
}

/**
 * @brief UART DMA 空闲中断接收
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle USART 外设句柄，指向 HAL UART_HandleTypeDef
 * @param[out] received_data 接收数据缓冲区
 * @param timeout_ms 超时时间（当前未使用）
 * @return 操作结果错误码
 */
inline ErrorCode uartReceiveToIdleDma(bool, void* handle, std::span<uint8_t> received_data, uint32_t)
{
    const auto status = HAL_UARTEx_ReceiveToIdle_DMA(static_cast<UART_HandleTypeDef*>(handle),
                                                     received_data.data(),
                                                     received_data.size_bytes());
    return detail::halStatusToErrorCode(status);
}

/**
 * @brief UART 阻塞发送
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle USART 外设句柄，指向 HAL UART_HandleTypeDef
 * @param data 待发送数据
 * @param timeout_ms 超时时间（毫秒）
 * @return 操作结果错误码
 */
inline ErrorCode uartTransmitBlocking(bool, void* handle, std::span<const uint8_t> data, const uint32_t timeout_ms)
{
    const auto status =
        HAL_UART_Transmit(static_cast<UART_HandleTypeDef*>(handle), data.data(), data.size_bytes(), timeout_ms);
    return detail::halStatusToErrorCode(status);
}

/**
 * @brief UART 中断模式发送
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle USART 外设句柄，指向 HAL UART_HandleTypeDef
 * @param data 待发送数据
 * @param timeout_ms 超时时间（当前未使用）
 * @return 操作结果错误码
 * @retval ErrorCode::PermissionDenied USART 非就绪状态
 */
inline ErrorCode uartTransmitInterrupt(bool, void* handle, std::span<const uint8_t> data, uint32_t)
{
    if (HAL_UART_GetState(static_cast<UART_HandleTypeDef*>(handle)) != HAL_UART_STATE_READY) {
        return ErrorCode::PermissionDenied;
    }

    const auto status = HAL_UART_Transmit_IT(static_cast<UART_HandleTypeDef*>(handle), data.data(), data.size_bytes());
    return detail::halStatusToErrorCode(status);
}

/**
 * @brief UART DMA 模式发送
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param[in] handle USART 外设句柄，指向 HAL UART_HandleTypeDef
 * @param data 待发送数据
 * @param timeout_ms 超时时间（当前未使用）
 * @return 操作结果错误码
 * @retval ErrorCode::PermissionDenied USART 或 DMA 发送通道非就绪状态
 */
inline ErrorCode uartTransmitDma(bool, void* handle, std::span<const uint8_t> data, uint32_t)
{
    if (HAL_UART_GetState(static_cast<UART_HandleTypeDef*>(handle)) != HAL_UART_STATE_READY) {
        return ErrorCode::PermissionDenied;
    }
    if (HAL_DMA_GetState(static_cast<UART_HandleTypeDef*>(handle)->hdmatx) != HAL_DMA_STATE_READY) {
        return ErrorCode::PermissionDenied;
    }

    const auto status = HAL_UART_Transmit_DMA(static_cast<UART_HandleTypeDef*>(handle), data.data(), data.size_bytes());
    return detail::halStatusToErrorCode(status);
}

}  // namespace emdevif::stm32hal

#endif  // !EMDEVIF_STM32_PERIPHERAL_HAL_USART_HPP
