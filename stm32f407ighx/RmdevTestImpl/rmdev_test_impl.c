/**
 * @file rmdev_test_impl.c
 * @author DuYicheng
 * @date 2025-07-30
 * @brief 测试函数的实现
 */

#include "rmdev_test_impl.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "usart.h"
#include "emdevif_test_framework.h"

extern void rmdev_testEntry(void);

static void test_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char buffer[256];
    sprintf(buffer, format, args);
    va_end(args);

    while (HAL_UART_GetState(&huart6) != HAL_UART_STATE_READY) {
    }

    for (size_t i = 0; i < strlen(buffer); i++) {
        const uint8_t c = buffer[i];
        HAL_UART_Transmit(&huart6, &c, 1, HAL_MAX_DELAY);
    }
}

void testEntry(void)
{
    const emdevif_test_Callbacks callbacks = {.printfCallback = test_printf, .testEntryCallback = rmdev_testEntry};
    emdevif_test_framework_main("\r\n", &callbacks, NULL);
}
