/**
 * @file INS_task.cppm
 * @brief 姿态解算任务
 */

module;

#include <cstdint>
#include <cstring>

#include <algorithm>
#include <span>

#include "printf.h"

#include "FreeRTOS.h"
#include "task.h"

#include "emdevif/core/attributes_and_useful_macros.h"
#include "emdevif/core/fatal_handler.h"

export module ins_task;

import emdevif.core.error_handler;
import emdevif.system.thread;
import emdevif.timeline;

import emdevif.peripheral.pwm;
import emdevif.peripheral.gpio;
import emdevif.peripheral.spi;
import emdevif.peripheral.serial;

import rmdev.driver.imu.bmi088;
import rmdev.control_algorithm.pid;
import rmdev.math;
import rmdev.debug_assistance.vofa;
import rmdev.ins;

export EMDEVIF_NO_RETURN void insTask(void*)
{
    using namespace emdevif;

    static const emdevif::Serial ins_result_serial{"INS result transmit serial"};
    static const emdevif::Serial ins_log_serial{"test transmit serial"};

    static const emdevif::Pwm bmi088_heat_pwm{"BMI088 heat PWM"};
    static rmdev::Pid bmi088_heat_ctrl_pid{{.kp = 20.0f, .ki = 1.0f, .kd = 0.0f}, 100.0f, 75.0f};

    static rmdev::Bmi088 bmi088{Spi{"BMI088 communicate SPI"}, Gpio{"BMI088 SPI accel cs"}, Gpio{"BMI088 SPI gyro cs"}};

    bmi088_heat_pwm.enable();
    bmi088.deviceInit(true);

    static rmdev::Ins ins;
    ins.init();

    char msg[64];
    ::sprintf(msg, "[INS task]: sizeof(ins) = %zu", sizeof ins);
    const auto p_msg = reinterpret_cast<uint8_t*>(msg);
    ins_log_serial.transmit(false, {p_msg, std::strlen(msg)}, Serial::max_delay);

    while (true) {
        uint_fast16_t tick = 0;

        bmi088.readImuData();

        auto& imu_data = bmi088.getImuData();

        ins.insUpdate(imu_data);

        EMDEVIF_DATA_SECTION(".ram_d1") static std::array<float, 10 + 1> imu_data_buffer;

        if (ins_result_serial.getStatus(false) == Serial::Ready) {
            imu_data_buffer = {imu_data.accel[0],
                               imu_data.accel[1],
                               imu_data.accel[2],
                               imu_data.gyro[0],
                               imu_data.gyro[1],
                               imu_data.gyro[2],
                               imu_data.temperature,
                               imu_data.yaw,
                               imu_data.pitch,
                               imu_data.roll};
        }

        const auto buf = rmdev::debug_assistance::vofa::JustFloat::processData(imu_data_buffer, 10);
        (void)ins_result_serial.transmit(false, buf, 0);

        if (tick % 2 == 0) {
            bmi088_heat_ctrl_pid(40.0f, bmi088.getImuData().temperature);

            const auto v = static_cast<uint8_t>(std::clamp(bmi088_heat_ctrl_pid.getOutput(), 0.0f, 100.0f));
            bmi088_heat_pwm.setRatio(v);
        }

        ++tick;
        Thread::delay(1);
    }
}
