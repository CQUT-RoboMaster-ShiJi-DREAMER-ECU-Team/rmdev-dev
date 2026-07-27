/**
 * @file attributes_and_useful_macros.h
 * @brief 属性与常用宏
 */

#pragma once
#ifndef EMDEVIF_CORE_ATTRIBUTES_AND_USEFUL_MACROS_H
#define EMDEVIF_CORE_ATTRIBUTES_AND_USEFUL_MACROS_H

// 更简洁地实现 extern "C"
#ifdef __cplusplus
#define EMDEVIF_EXTERN_C       extern "C"
#define EMDEVIF_EXTERN_C_BEGIN extern "C" {
#define EMDEVIF_EXTERN_C_END   }
#else  // !__cplusplus
#define EMDEVIF_EXTERN_C_BEGIN
#define EMDEVIF_EXTERN_C_END
#define EMDEVIF_EXTERN_C
#endif  // !__cplusplus

/* Attributes ================================================================================== */
#include "emdevif/core/detail/attribute_implements/clang_attributes.h"
#include "emdevif/core/detail/attribute_implements/gnu_attributes.h"
#include "emdevif/core/detail/attribute_implements/msvc_attributes.h"

/* Useful Macros =============================================================================== */

// 通过将变量强制转换为 void 类型来实现取消编译器警告
#define EMDEVIF_UNUSED(X)      (void)X

/// 计算 C 风格数组的元素个数
/// @note 如果是 C++ 调用，建议使用 std::size （在头文件 <iterator> 中）代替这个宏
#define EMDEVIF_COUNTOF(array) (sizeof(array) / sizeof(array[0]))

#endif  // !EMDEVIF_CORE_ATTRIBUTES_AND_USEFUL_MACROS_H
