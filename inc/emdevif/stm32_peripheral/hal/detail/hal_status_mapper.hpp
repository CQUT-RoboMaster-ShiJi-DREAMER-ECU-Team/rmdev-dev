/**
 * @file hal_status_mapper.hpp
 * @brief 将 HAL_Status 映射到 emdevif::ErrorCode
 */

#pragma once
#ifndef EMDEVIF_STM32_PERIPHERAL_HAL_DETAIL_HAL_STATUS_MAPPER_HPP
#define EMDEVIF_STM32_PERIPHERAL_HAL_DETAIL_HAL_STATUS_MAPPER_HPP

#include <cstdint>

#include "main.h"

#include "emdevif/core/error_handler.hpp"

namespace emdevif::stm32hal::detail {

/**
 * @brief 将 STM32 HAL 状态码转换为 emdevif 统一错误码
 * @param hal_status STM32 HAL 库返回的状态值
 * @return 对应的 emdevif ErrorCode
 * @retval ErrorCode::Success HAL_OK 映射
 * @retval ErrorCode::UnknownError HAL_ERROR 映射
 * @retval ErrorCode::OperationFail HAL_BUSY 映射
 * @retval ErrorCode::Timeout HAL_TIMEOUT 映射
 * @retval ErrorCode::InternalError 未知状态默认映射
 */
constexpr ErrorCode halStatusToErrorCode(const HAL_StatusTypeDef hal_status)
{
    switch (hal_status) {
    case HAL_OK:
        return ErrorCode::Success;
    case HAL_ERROR:
        return ErrorCode::UnknownError;
    case HAL_BUSY:
        return ErrorCode::OperationFail;
    case HAL_TIMEOUT:
        return ErrorCode::Timeout;
    default:
        return ErrorCode::InternalError;
    }
}

}  // namespace emdevif::stm32hal::detail

#endif  // !EMDEVIF_STM32_PERIPHERAL_HAL_DETAIL_HAL_STATUS_MAPPER_HPP