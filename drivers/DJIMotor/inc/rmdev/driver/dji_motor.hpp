/**
 * @file dji_motor.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_DRIVER_DJI_MOTOR_HPP
#define RMDEV_DRIVER_DJI_MOTOR_HPP

#include <cstdint>

#include <algorithm>
#include <compare>
#include <concepts>
#include <numbers>
#include <span>
#include <string_view>

#include <mp-units/systems/si.h>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/concepts.hpp"
#include "emdevif/core/error_handler.hpp"
#include "emdevif/peripheral/can.hpp"
#include "rmdev/device_model/motor.hpp"
#include "rmdev/driver/dji_motor/can_address.hpp"
#include "rmdev/driver/dji_motor/class_dji_motor.hpp"
#include "rmdev/driver/dji_motor/dji_motor_group.hpp"

#endif  // !RMDEV_DRIVER_DJI_MOTOR_HPP
