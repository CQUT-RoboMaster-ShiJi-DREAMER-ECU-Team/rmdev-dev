/**
 * @file dji_motor.cppm
 * @brief
 */

module;

#include "rmdev/driver/dji_motor.hpp"

export module rmdev.driver.dji_motor;

export namespace rmdev {
using ::rmdev::DJIMotor;
using ::rmdev::DJIMotorFeedbackCanBase;
using ::rmdev::DJIMotorGroup;
using ::rmdev::DJIMotorGroupBuilder;
using ::rmdev::DJIMotorTransmitCanBase;
using ::rmdev::IsDJIMotor;
using ::rmdev::ValidDJIMotor;
}  // namespace rmdev
