/**
 * @file Motor.cppm
 * @brief 电机的抽象模型
 */

module;

#include "rmdev/device_model/motor.hpp"

export module rmdev.device_model.motor;

export namespace rmdev::units::motor_units {
using ::rmdev::units::motor_units::rpm;
}
export namespace rmdev {
using ::rmdev::MotorGroup;
using ::rmdev::MotorRotateRecorder;
}  // namespace rmdev
