/**
 * @file can_hal_impl.cppm
 * @brief STM32 HAL 库的 CAN 收发函数的封装，用以 emdevif::Can 类的回调函数
 */

module;

#include "can.h"
#include "emdevif/stm32_peripheral/hal/can.hpp"

export module emdevif.stm32_peripheral.hal.can;

export namespace emdevif::stm32hal {
using ::emdevif::stm32hal::canAddTxMessage;
}
