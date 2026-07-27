/**
 * @file fatal_handler.hpp
 * @brief 致命错误处理
 */

#pragma once
#ifndef EMDEVIF_CORE_ERROR_HANDLER_FATAL_HANDLER_HPP
#define EMDEVIF_CORE_ERROR_HANDLER_FATAL_HANDLER_HPP

#include <cstdarg>

#include <type_traits>

#include "emdevif/core/attributes_and_useful_macros.h"

namespace emdevif {

/**
 * 终止程序的函数指针类型
 */
using TerminateFunction = void (*)();

/**
 * 终止整个程序
 */
EMDEVIF_NO_RETURN void terminate();

/**
 * 注册终止函数（不具备线程安全性，建议只在初始化的时候调用）。
 * 如果不调用这个函数，那么将调用默认的终止函数。
 * @param func 终止函数。注意该函数不能返回，否则是未定义行为。
 */
void registerTerminateFunction(TerminateFunction func) noexcept;

/**
 * 致命错误回调函数指针
 * @param file 将会传入致命错误产生的文件名称
 * @param line 将会传入致命错误产生的行号
 * @param format 将会传入消息的格式化字符串
 * @param args 将会传入填充占位符的可变参列表
 */
using FatalHandlerCallBack = void (*)(const char* file, int line, const char* format, std::va_list args);

/**
 * 致命错误处理函数
 * @attention 不应当直接使用这个函数，应该使用宏 EMDEVIF_FATAL_HANDLER，
 * 可以自动填充文件名与行号，但需要引入头文件 @ref emdevif/core/fatal_handler.hpp
 * @param file 所在文件名
 * @param line 所在行号
 * @param format 错误信息的格式化字符串
 * @param args 填充占位符的可变参列表
 */
EMDEVIF_NO_RETURN void fatalHandler(const char* file, int line, const char* format, std::va_list args) noexcept;

/**
 * @overload
 * @param ... 填充占位符的值
 */
EMDEVIF_NO_RETURN void fatalHandler(const char* file, int line, const char* format, ...) noexcept
    EMDEVIF_FORMAT_CHECK(printf, 3, 4);

/**
 * 注册致命错误回调函数（不具备线程安全性，建议只在初始化的时候调用）。
 * 如果不调用这个函数，那么将不会有额外的处理，直接调用 terminate。
 * @param callback 回调函数
 */
void registerFatalHandler(FatalHandlerCallBack callback) noexcept;

/**
 * 断言失败处理函数指针类型
 */
using AssertFailedHandler =
    void (*)(const char* file, int line, const char* func_name, const char* condition_name, const char* message);

namespace detail {

/**
 * 断言失败处理函数指针（编译期初始化为默认的断言失败处理函数）
 *
 * @attention 如果编译器报错提示 'emdevif::detail::assertFailedHandler@@emdevif.core.error_handler' was not declared
 * 'constexpr'，
 * 这不是框架的设计错误，而是某处的断言在常量求值上下文中失败了，需要用户检查函数传入的参数是否符合断言条件。
 */
extern AssertFailedHandler assertFailedHandler;

}  // namespace detail

/**
 * 断言入口
 * @attention 不应直接调用这个函数，应当使用宏 @ref EMDEVIF_ASSERT，可以自动填充文件名、行号、函数名与表达式名称，
 * 但需要引入头文件 @ref emdevif/core/fatal_handler.h
 * @note 为了避免与 <cassert> 中的 assert 冲突，故将此函数命名为 emdevif_assert
 *
 * @param condition 待断言的表达式
 * @param file 调用函数所在文件
 * @param line 调用函数所在行号
 * @param func_name 包含函数名称
 * @param condition_name 表达式名称
 * @param message 信息
 */
constexpr void emdevif_assert(const bool condition,
                              const char* file,
                              const int line,
                              const char* func_name,
                              const char* condition_name,
                              const char* message = "") noexcept
{
    // 把这个函数设置为 constexpr 是为了在常量求值上下文中也能够使用

    // clang-format off

    // 以下的写法可能令人困惑，这是为了确保该函数在常量求值上下文中也能够正常使用而设计的（参考了标准库 cassert 中 assert 宏的实现）
    (void)(
        condition ||  // 利用了内置的 || 运算符的“短路求值”特性，如果 condition 是 true，则直接忽略 || 后续的判断，
                       // 这样，在常量求值上下文中就不会对 detail::assertFailedHandler 进行访问，从而避免了在常量
                       // 求值上下文中访问非常量表达式的变量导致的编译错误。

                       // 相应地，如果 condition 为 false，就会访问非常量表达式 detail::assertFailedHandler，
                       // 从而触发编译错误，提示用户在常量求值上下文中存在断言失败。

                       // 也就是说，如果编译器报错提示“此处使用了非常量表达式变量 detail::assertFailedHandler”，
                       // 这不是框架的设计错误，而是某处的断言在常量求值上下文中失败了，需要用户检查函数传入的参数是否符合规定。
        (detail::assertFailedHandler != nullptr ?
            (detail::assertFailedHandler(file, line, func_name, condition_name, message), false) :
            false
        ) ||
        (terminate(), false)
    );
    // 上述代码块的结构大致可以看作 `(void)(condition || B || C)`
    // 其中 B 块是一个始终返回 false 的表达式（? : 中间的表达式用到了逗号运算符，取的是最后一个表达式 false 的值），
    // 从而确保最终调用 C 块的 terminate

    // 至于为什么要使用逗号运算符，这是因为 detail::assertFailedHandler 和 terminate 调用后返回的是 void，而
    // || 运算符两侧都必须是布尔类型的，因此使用逗号运算符后面接一个 false 来满足 || 运算符的类型要求，同时又不影响函数的逻辑。

    // clang-format on
}

/**
 * 断言失败处理函数注册（不具备线程安全性，建议只在初始化的时候调用）。
 * 如果不调用这个函数，那么将调用默认的断言失败处理函数。
 * @param handler 回调函数
 */
void registerAssertFailedHandler(AssertFailedHandler handler) noexcept;

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_ERROR_HANDLER_FATAL_HANDLER_HPP
