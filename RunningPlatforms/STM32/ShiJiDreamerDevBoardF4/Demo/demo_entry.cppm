/**
 * @file demo_entry.cppm
 * @brief 单元测试结束后的演示程序
 */

module;

#include <cstdint>

#include "emdevif/core/attributes_and_useful_macros.h"

#include "emdevif/core/fatal_handler.h"

export module afterUnitTestDemo;

import emdevif.core.error_handler;
import emdevif.sys.thread;
import emdevif.peripheral.pwm;
import emdevif.stm32_peripheral.hal.pwm;

export EMDEVIF_NO_RETURN void demoEntry() noexcept
{
    using namespace emdevif;

    Thread breathing_light_thread = Thread::create(
        {.name = "breathingLight", .priority = Thread::Priority::BelowNormal, .stack_size = 128},
        [](void*) noexcept {
            const emdevif::Pwm pwm{"breathing light pwm"};

            pwm.enable();
            while (true) {
                constexpr auto delay_ms = 70;
                constexpr auto step_ratio = 5;

                for (uint8_t i = 0; i < 100; i += step_ratio) {
                    pwm.setRatio(i);
                    Thread::delay(Thread::msToTick(delay_ms));
                }
                for (uint8_t i = 0; i < 100; i += step_ratio) {
                    pwm.setRatio(100 - i);
                    Thread::delay(Thread::msToTick(delay_ms));
                }
            }
        },
        nullptr);

    while (true) {
        Thread::delay(Thread::maxDelay());
    }
}
