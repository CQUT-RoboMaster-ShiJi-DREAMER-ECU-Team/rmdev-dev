/**
 * @file stm32_hal_test_func_impl.c
 * @author DuYicheng
 * @date 2025-07-30
 * @brief 测试函数的实现
 */

#include "stm32_hal_test_func_impl.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "usart.h"
#include "emdevif_test_framework.h"

extern void rmdev_testEntry(void);

static UART_HandleTypeDef* test_uart_handle = NULL;
static char printf_buffer[512];

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

void testInit(UART_HandleTypeDef* const test_huart)
{
    test_uart_handle = test_huart;
}

void testEntry(void)
{
    const emdevif_test_Callbacks callbacks = {.printfCallback = test_printf, .testEntryCallback = rmdev_testEntry};
    emdevif_test_framework_main("\r\n", &callbacks, NULL);
}
