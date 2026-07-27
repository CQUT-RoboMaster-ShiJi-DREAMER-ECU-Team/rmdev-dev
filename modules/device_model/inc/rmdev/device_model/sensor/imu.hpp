/**
 * @file imu.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_DEVICE_MODEL_SENSOR_IMU_HPP
#define RMDEV_DEVICE_MODEL_SENSOR_IMU_HPP

#include "emdevif/core/concepts.hpp"
namespace rmdev {

/**
 * 惯性测量单元（IMU）的抽象模型
 * @tparam T 存储数据的类型
 */
template<emdevif::ArithmeticType T>
struct Imu {
    using DataType = T;        ///< 存储数据的类型

    DataType accel[3];         ///< 加速度（0, 1, 2 号元素分别对应 X, Y, Z）
    DataType gyro[3];          ///< 角速度（0, 1, 2 号元素分别对应 X, Y, Z）
    DataType temperature;      ///< 温度

    DataType yaw;              ///< 航向角
    DataType pitch;            ///< 俯仰角
    DataType roll;             ///< 横滚角

    DataType yaw_total_angle;  ///< 航向角累计转过的角度
};

}  // namespace rmdev

#endif  // !RMDEV_DEVICE_MODEL_SENSOR_IMU_HPP
