/**
 * @file demo_entry.cppm
 * @author DuYicheng
 * @date 2025-10-05
 * @brief 单元测试结束后的演示程序
 */

module;

#include <cstdint>

#include "emdevif/attributes_and_useful_macros.h"

#include "emdevif/fatal_handler.hpp"

export module afterUnitTestDemo;

import emdevif.errorHandler;
import emdevif.sys.thread;

import emdevif.peripheral.pwm;
import emdevif.peripheral.gpio;
import emdevif.peripheral.spi;

emdevif::Gpio bmi088_accel_cs{"BMI088 SPI accel cs"};

export EMDEVIF_NO_RETURN void demoEntry() noexcept
{
    using namespace emdevif;

    while (true) {
        Thread::delay(Thread::maxDelay());
    }
}
