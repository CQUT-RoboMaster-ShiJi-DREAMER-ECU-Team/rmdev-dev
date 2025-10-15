/**
 * @file INS_task.cppm
 * @author DuYicheng
 * @date 2025-10-15
 * @brief 姿态解算任务
 */

module;

#include <cstdint>

#include <algorithm>

#include "emdevif/attributes_and_useful_macros.h"
#include "emdevif/fatal_handler.hpp"

export module ins_task;

import emdevif.errorHandler;
import emdevif.sys.thread;
import emdevif.timeline;

import emdevif.peripheral.pwm;
import emdevif.peripheral.gpio;
import emdevif.peripheral.spi;

import rmdev.driver.imu.bmi088;
import rmdev.controlAlgorithm.pid;
import rmdev.util.math;

export EMDEVIF_NO_RETURN void insTask(void*)
{
    using namespace emdevif;

    const emdevif::Pwm bmi088_heat_pwm{"BMI088 heat PWM"};
    rmdev::Pid bmi088_heat_ctrl_pid{{.kp = 0.005f, .ki = 0.0f, .kd = 0.0f}, 100.0f, 100.0f};

    rmdev::Bmi088 bmi088{Spi{"BMI088 communicate SPI"}, Gpio{"BMI088 SPI accel cs"}, Gpio{"BMI088 SPI gyro cs"}};

    bmi088_heat_pwm.enable();
    while (true) {
        uint_fast16_t tick = 0;

        bmi088.readImuData();

        if (tick % 2 == 0) {
            bmi088_heat_ctrl_pid(40.0f, bmi088.getImuData().temperature);

            const auto v = static_cast<uint8_t>(std::clamp(bmi088_heat_ctrl_pid.getOutput(), 0.0f, 100.0f));
            bmi088_heat_pwm.setRatio(v);
        }

        ++tick;
        Thread::delay(1);
    }
}
