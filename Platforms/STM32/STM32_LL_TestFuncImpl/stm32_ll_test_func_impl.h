/**
 * @file stm32_ll_test_func_impl.c
 * @author DuYicheng
 * @date 2025-07-30
 * @brief 测试函数的实现
 */

#pragma once
#ifndef STM32_LL_TEST_FUNC_IMPL
#define STM32_LL_TEST_FUNC_IMPL

#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif

void testInit(USART_TypeDef* test_usart);
void testEntry(void);

#ifdef __cplusplus
}
#endif

#endif  // !STM32_LL_TEST_FUNC_IMPL
