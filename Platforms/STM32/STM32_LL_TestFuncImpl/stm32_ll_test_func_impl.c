/**
 * @file stm32_ll_test_func_impl.c
 * @author DuYicheng
 * @date 2025-07-30
 * @brief 测试函数的实现
 */

#include "stm32_ll_test_func_impl.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "usart.h"
#include "emdevif_test_framework.h"

#include "emdevif/line_separator.h"

extern void rmdev_testEntry(void);

static USART_TypeDef* usartx = NULL;
static char printf_buffer[512];

static void test_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    vsprintf(printf_buffer, format, args);
    va_end(args);

    for (size_t i = 0; i < strlen(printf_buffer); i++) {
        while (LL_USART_IsActiveFlag_TC(USART1) != 1) {
        }

        const uint8_t c = printf_buffer[i];
        LL_USART_TransmitData8(USART1, c);
    }
}

void testInit(USART_TypeDef* const test_usart)
{
    usartx = test_usart;
}

void testEntry(void)
{
    const emdevif_test_Callbacks callbacks = {.printfCallback = test_printf, .testEntryCallback = rmdev_testEntry};
    emdevif_test_framework_main(EMDEVIF_LINE_SEPARATOR, &callbacks, NULL);
}
