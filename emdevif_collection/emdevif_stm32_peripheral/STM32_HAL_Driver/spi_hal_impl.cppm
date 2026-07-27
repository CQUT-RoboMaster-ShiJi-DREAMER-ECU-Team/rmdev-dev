/**
 * @file spi_hal_impl.cppm
 * @brief STM32 HAL 库的 SPI 收发函数的封装，用以 emdevif::Spi 类的回调函数
 */

module;

#include "spi.h"
#include "emdevif/stm32_peripheral/hal/spi.hpp"

export module emdevif.stm32_peripheral.hal.spi;

export namespace emdevif::stm32hal {
using ::emdevif::stm32hal::spiTransmitReceiveBlock;
using ::emdevif::stm32hal::spiTransmitReceiveDma;
using ::emdevif::stm32hal::spiTransmitReceiveInterrupt;
}  // namespace emdevif::stm32hal
