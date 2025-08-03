/**
 * @file stm32_hal_test_func_impl.c
 * @author DuYicheng
 * @date 2025-07-30
 * @brief 测试函数的实现
 */

#pragma once
#ifndef STM32_HAL_TEST_FUNC_IMPL
#define STM32_HAL_TEST_FUNC_IMPL

#include "usart.h"

void testInit(UART_HandleTypeDef* test_huart);
void testEntry(void);

#endif  // !STM32_HAL_TEST_FUNC_IMPL
