/**
 * @file can.hpp
 * @brief STM32 HAL CAN 外设适配，封装 HAL CAN 发送函数供 emdevif::Can 使用
 */

#pragma once
#ifndef EMDEVIF_STM32_PERIPHERAL_HAL_CAN_HPP
#define EMDEVIF_STM32_PERIPHERAL_HAL_CAN_HPP

#include <cstdint>

#include <span>

#include "can.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/peripheral/model/can.hpp"
#include "emdevif/stm32_peripheral/hal/detail/hal_status_mapper.hpp"

namespace emdevif::stm32hal {

/**
 * @brief 向 CAN 总线发送一帧数据
 * @param in_isr 是否在中断上下文中调用（当前未使用）
 * @param handle CAN 外设句柄，指向 HAL CAN_HandleTypeDef
 * @param header CAN 数据帧头信息（ID、IDE、RTR、DLC）
 * @param data 待发送的数据负载
 * @return 操作结果错误码
 * @retval ErrorCode::Success 发送成功
 * @retval ErrorCode::UnknownError HAL_ERROR 错误
 * @retval ErrorCode::OperationFail HAL_BUSY 错误
 * @retval ErrorCode::Timeout HAL_TIMEOUT 超时
 * @retval ErrorCode::InternalError 未知状态
 */
inline ErrorCode canAddTxMessage(bool,
                                 void* handle,
                                 const CanModel::DataHeader& header,
                                 std::span<const uint8_t> data) noexcept
{
    uint32_t tx_mailbox;

    CAN_TxHeaderTypeDef hal_tx_header{.IDE = static_cast<uint32_t>(header.ide),
                                      .RTR = static_cast<uint32_t>(header.rtr),
                                      .DLC = static_cast<uint32_t>(header.dlc)};
    if (header.ide == CanModel::DataIdentifier::StandardId) {
        hal_tx_header.StdId = header.id;
    }
    else if (header.ide == CanModel::DataIdentifier::ExtendedId) {
        hal_tx_header.ExtId = header.id;
    }

    const auto status = HAL_CAN_AddTxMessage(handle, &hal_tx_header, data.data(), &tx_mailbox);
    return detail::halStatusToErrorCode(status);
}

}  // namespace emdevif::stm32hal

#endif  // !EMDEVIF_STM32_PERIPHERAL_HAL_CAN_HPP
