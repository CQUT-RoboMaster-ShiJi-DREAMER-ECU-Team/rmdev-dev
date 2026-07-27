/**
 * @file can_address.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_DRIVER_DJI_MOTOR_CAN_ADDRESS_HPP
#define RMDEV_DRIVER_DJI_MOTOR_CAN_ADDRESS_HPP

#include <cstdint>
namespace rmdev {

/// @brief 大疆电机的发送报文标识符
/// @attention C620 与 C610 的发送、反馈报文地址完全相同；
///            要注意 GM6020 与 C610/C620 的报文地址有可能重合
enum class DJIMotorTransmitCanBase : uint_fast16_t {
    /// @brief ID 为 1, 2, 3, 4 的 GM6020 电机对应的标识符
    /// @note 电流给定值范围：-16384 ~ 0 ~ 16384，对应最大转矩电流范围 -3A ~ 0 ~ 3A
    GM6020_TX1234 = 0x1FE,

    /// @brief ID 为 5, 6, 7 的 GM6020 电机对应的标识符
    /// @note 电流给定值范围：-16384 ~ 0 ~ 16384，对应最大转矩电流范围 -3A ~ 0 ~ 3A
    GM6020_TX567 = 0x2FE,

    /// @brief ID 为 1, 2, 3, 4 的 C620 电调对应的标识符
    /// @note 控制电流值范围 -16384 ~ 0 ~ 16384，
    ///       对应电调输出的转矩电流范围 -20 ~ 0 ~ 20A
    C620_TX1234 = 0x200,

    /// @brief ID 为 5, 6, 7, 8 的 C620 电调对应的标识符
    /// @note 控制电流值范围 -16384 ~ 0 ~ 16384，
    ///       对应电调输出的转矩电流范围 -20 ~ 0 ~ 20A
    C620_TX5678 = 0x1FF,

    /// @brief ID 为 1, 2, 3, 4 的 C610 电调对应的标识符
    /// @note 控制电流值范围 -10000 ~ 0 ~ 10000,
    ///       对应电调输出的转矩电流范围 -10 ~ 0 ~ 10A
    C610_TX1234 = 0x200,

    /// @brief ID 为 5, 6, 7, 8 的 C610 电调对应的标识符
    /// @note 控制电流值范围 -10000 ~ 0 ~ 10000,
    ///       对应电调输出的转矩电流范围 -10 ~ 0 ~ 10A
    C610_TX5678 = 0x1FF
};

/// @brief 大疆电机/电调的 CAN 反馈报文标识符基址
enum class DJIMotorFeedbackCanBase : uint_fast16_t {
    /// @brief GM6020 电机的反馈报文标识符
    GM6020 = 0x204,

    /// @brief C620 电调的反馈报文标识符
    C620 = 0x200,

    /// @brief C610 电调的反馈报文标识符
    C610 = 0x200
};

}  // namespace rmdev

#endif  // !RMDEV_DRIVER_DJI_MOTOR_CAN_ADDRESS_HPP
