/**
 * @file stm32_hal_test_func_impl.cpp
 * @author DuYicheng
 * @date 2025-07-30
 * @brief 测试函数的实现
 */

#include "stm32_hal_test_func_impl.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdint>

#include "emdevif_test_framework.h"

#include "emdevif/attributes_and_useful_macros.h"
#include "emdevif/fatal_handler.hpp"

import emdevif.errorHandler;
import emdevif.sys.thread;
import emdevif.stm32Peripheral.hal.usart;
import emdevif.connectivity.serial;

extern "C" void rmdev_testEntry(void);  // NOLINT(*-redundant-void-arg)

constinit emdevif::Serial test_tx_serial{
    "test transmit serial",
    {.receive_function = emdevif::Serial::noReceive, .transmit_function = emdevif::stm32hal::uartTransmitBlocking}};

static char printf_buffer[512];

static constexpr auto DEFAULT_TASK_STACK_DEPTH = 1024U;
static emdevif::Thread::StaticInstance<DEFAULT_TASK_STACK_DEPTH> default_task_static_instance;
static emdevif::Thread default_task;

EMDEVIF_NO_RETURN static void osStartDefaultTask(void* arguments);

static void test_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    vsprintf(printf_buffer, format, args);
    va_end(args);

    const auto tx_ptr = reinterpret_cast<const uint8_t*>(printf_buffer);
    test_tx_serial.transmit(false, {tx_ptr, strlen(printf_buffer)}, emdevif::stm32hal::uart_max_delay);
}

extern "C" EMDEVIF_NO_RETURN void testEntry(void)
{
    emdevif::registerFatalHandler([](const char* file, const int line, const char* message) {
        test_printf("emdevif: Fatal touched at {}:{}\r\n\r\n", file, line);
        test_printf("Message: {}", message);
        while (true) {
        }
    });

    default_task = emdevif::Thread::create({.name = "DefaultTask",
                                            .priority = emdevif::Thread::Priority::Max,
                                            .static_instance = &default_task_static_instance,
                                            .stack_size = DEFAULT_TASK_STACK_DEPTH},
                                           osStartDefaultTask,
                                           nullptr);
    if (!default_task.getHandle().has_value()) {
        EMDEVIF_FATAL_HANDLER("Failed to create default task.");
    }

    emdevif::Thread::startScheduler();

    // 程序不应当执行到此处
    while (true) {
    }
}

EMDEVIF_NO_RETURN static void osStartDefaultTask(void* arguments)
{
    EMDEVIF_UNUSED(arguments);

    constexpr emdevif_test_Callbacks callbacks = {.printfCallback = test_printf,
                                                  .testEntryCallback = rmdev_testEntry,
                                                  .testFinishCallback = nullptr,
                                                  .errorCallback = nullptr};
    emdevif_test_framework_main("\r\n", &callbacks, nullptr);

    while (true) {
        emdevif::Thread::delay(1);
    }
}
