/**
 * @file demo_entry.cppm
 * @author NaAlO2
 * @date 2025-10-05
 * @brief
 */

module;

#include <cstdint>

#include "emdevif/attributes_and_useful_macros.h"

#include "emdevif/fatal_handler.hpp"

export module afterUnitTestDemo;

import emdevif.errorHandler;
import emdevif.sys.thread;
import emdevif.peripheralModels.pwm;
import emdevif.stm32Peripheral.hal.pwm;

export EMDEVIF_NO_RETURN void demoEntry() noexcept
{
    using namespace emdevif;

    Thread breathing_light_thread = Thread::create(
        {.name = "breathingLight", .priority = Thread::Priority::BelowNormal, .stack_size = 128},
        [](void*) noexcept {
            const emdevif::Pwm pwm{"breathing light pwm",
                                   stm32hal::pwmEnable,
                                   stm32hal::pwmDisable,
                                   stm32hal::setRatio};

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
