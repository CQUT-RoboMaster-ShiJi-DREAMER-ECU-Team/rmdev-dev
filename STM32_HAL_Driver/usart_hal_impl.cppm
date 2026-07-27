/**
 * @file usart_hal_impl.cppm
 * @brief STM32 HAL 库的 USART 收发函数的封装，用以 emdevif::Serial 类的回调函数
 */

module;

#include "usart.h"
#include "emdevif/stm32_peripheral/hal/usart.hpp"

export module emdevif.stm32_peripheral.hal.usart;

export namespace emdevif::stm32hal {
using ::emdevif::stm32hal::uart_max_delay;
using ::emdevif::stm32hal::uart_none_blocking;
using ::emdevif::stm32hal::uartGetState;
using ::emdevif::stm32hal::uartReceiveBlocking;
using ::emdevif::stm32hal::uartReceiveDma;
using ::emdevif::stm32hal::uartReceiveInterrupt;
using ::emdevif::stm32hal::uartReceiveToIdleDma;
using ::emdevif::stm32hal::uartTransmitBlocking;
using ::emdevif::stm32hal::uartTransmitDma;
using ::emdevif::stm32hal::uartTransmitInterrupt;
}  // namespace emdevif::stm32hal
