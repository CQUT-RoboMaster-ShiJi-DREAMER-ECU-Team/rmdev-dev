/**
 * @file fatal_handler.h
 * @brief 致命错误宏
 */

#pragma once
#ifndef EMDEVIF_CORE_FATAL_HANDLER_H
#define EMDEVIF_CORE_FATAL_HANDLER_H

#include <stdbool.h>  // NOLINT

#include "emdevif/core/attributes_and_useful_macros.h"

#if (!defined(__cplusplus) || defined(EMDEVIF_CORE_ERROR_HANDLER_IN_C_API_WRAPPER_SRC_))
/**
 * 供 C 语言调用的断言函数的参数类型
 */
typedef struct emdevif_AssertFuncParam_ {
    const bool condition;        ///< 待断言的表达式的真伪
    const char* file;            ///< 所在文件名
    const int line;              ///< 所在行号
    const char* func_name;       ///< 所在的函数名称
    const char* condition_name;  ///< 表达式字符串
    const char* message;         ///< 额外信息
} emdevif_AssertFuncParam_;
#endif

#ifndef __cplusplus

EMDEVIF_NO_RETURN void emdevif_fatal_handler_(const char* file, int line, const char* format, ...)
    EMDEVIF_FORMAT_CHECK(printf, 3, 4);

void emdevif_assert_(emdevif_AssertFuncParam_ param);

#if (__STDC_VERSION__ >= 202311L)
#define EMDEVIF_FATAL_HANDLER(format, ...) emdevif_fatal_handler_(__FILE__, __LINE__, format __VA_OPT__(, ) __VA_ARGS__)
#elif (defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER))
#define EMDEVIF_FATAL_HANDLER(format, ...) emdevif_fatal_handler_(__FILE__, __LINE__, format, ##__VA_ARGS__)
#endif

#ifdef NDEBUG
#define EMDEVIF_ASSERT(cond, ...) ((void)(cond))
#else
#define EMDEVIF_ASSERT(cond, ...)                                       \
    emdevif_assert_((emdevif_AssertFuncParam_){.condition = (cond),     \
                                               .file = __FILE__,        \
                                               .line = __LINE__,        \
                                               .func_name = __func__,   \
                                               .condition_name = #cond, \
                                               __VA_ARGS__})
#endif
/*
 * 此处使用了以下特性在 C 语言中模拟了 C++ 的默认参数：
 * 1、结构体初始化器可以尾随逗号，从而避免使用 ##__VA_ARGS__ 扩展语法。
 * 2、所有未显式初始化的成员都进行空初始化。如果 __VA_ARGS__ 为空，那么恰好最后一个结构体成员 message
 *    不会显式初始化，从而会被初始化为 0（NULL）
 */

#else  // __cplusplus

#define EMDEVIF_FATAL_HANDLER(format, ...) \
    ::emdevif::fatalHandler(__FILE__, __LINE__, format __VA_OPT__(, ) __VA_ARGS__)

#ifdef NDEBUG
#define EMDEVIF_ASSERT(condition, ...) ((void)(condition))
#else
#define EMDEVIF_ASSERT(condition, ...) \
    ::emdevif::emdevif_assert((condition), __FILE__, __LINE__, __func__, #condition __VA_OPT__(, ) __VA_ARGS__)
#endif

#endif  // __cplusplus

#endif  // !EMDEVIF_CORE_FATAL_HANDLER_H

/**
 * @def EMDEVIF_FATAL_HANDLER(format, ...)
 *
 * emdevif 致命错误处理宏
 * @attention C++ 中，需要导入模块 @ref emdevif.core.error_handler 才能正常使用这个宏，C语言不需要。
 * @param format 错误信息的格式化字符串
 * @param ... 填充占位符的值
 */

/**
 * @def EMDEVIF_ASSERT(condition, ...)
 *
 * @brief emdevif 断言宏
 * @param condition 断言条件
 * @param ... 断言失败时的错误信息字符串（可以省略不写）
 * @attention @arg 如果 @c NDEBUG 宏被定义，则此宏不进行任何操作
 *            @arg C++ 中，需要导入模块 @ref emdevif.core.error_handler 才能正常使用这个宏，C语言不需要
 */
