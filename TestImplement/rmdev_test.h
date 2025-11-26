/**
 * @file rmdev_test.h
 * @author DuYicheng
 * @date 2025-07-30
 * @brief 测试函数的实现
 */

#pragma once
#ifndef RMDEV_RMDEV_TEST_H
#define RMDEV_RMDEV_TEST_H

#include "emdevif/attributes_and_useful_macros.h"

EMDEVIF_EXTERN_C_BEGIN

void testInit(void* argument, ...);
EMDEVIF_NO_RETURN void testEntry(void);

EMDEVIF_EXTERN_C_END

#endif  // !RMDEV_RMDEV_TEST_H
