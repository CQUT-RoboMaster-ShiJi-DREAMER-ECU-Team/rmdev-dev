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

#include "usart.h"
#include "emdevif_test_framework.h"

#include "emdevif/attributes_and_useful_macros.h"
#include "emdevif/fatal_handler.hpp"

#include <optional>

import emdevif.error_handler;
import emdevif.sys.thread;

extern "C" void rmdev_testEntry(void);  // NOLINT(*-redundant-void-arg)

static UART_HandleTypeDef* test_uart_handle = nullptr;
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

    while (HAL_UART_GetState(test_uart_handle) != HAL_UART_STATE_READY) {
    }

    for (size_t i = 0; i < strlen(printf_buffer); i++) {
        const uint8_t c = printf_buffer[i];
        HAL_UART_Transmit(test_uart_handle, &c, sizeof c, HAL_MAX_DELAY);
    }
}

static void testInit(UART_HandleTypeDef* const test_huart)
{
    test_uart_handle = test_huart;
}

extern "C" EMDEVIF_NO_RETURN void testEntry(void)
{
    default_task = emdevif::Thread::create({.name = "DefaultTask",
                                            .priority = emdevif::Thread::Priority::Max,
                                            .static_instance = &default_task_static_instance,
                                            .stack_size = DEFAULT_TASK_STACK_DEPTH},
                                           osStartDefaultTask,
                                           nullptr);
    default_task.getHandle().or_else([] -> std::optional<void*> {
        EMDEVIF_FATAL_HANDLER("Failed to create default task.");
        return std::nullopt;
    });

    emdevif::Thread::startScheduler();

    // 程序不应当执行到此处
    while (true) {
    }
}

EMDEVIF_NO_RETURN static void osStartDefaultTask(void* arguments)
{
    EMDEVIF_UNUSED(arguments);

    testInit(&huart6);

    constexpr emdevif_test_Callbacks callbacks = {.printfCallback = test_printf,
                                                  .testEntryCallback = rmdev_testEntry,
                                                  .testFinishCallback = nullptr,
                                                  .errorCallback = nullptr};
    emdevif_test_framework_main("\r\n", &callbacks, nullptr);

    while (true) {
        emdevif::Thread::delay(1);
    }
}
