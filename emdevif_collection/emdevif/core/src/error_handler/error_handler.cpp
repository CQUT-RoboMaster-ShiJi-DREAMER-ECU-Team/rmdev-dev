/**
 * @file error_handler.cpp
 * @brief 错误处理
 */

#include <cstdarg>

#include "emdevif/core/attributes_and_useful_macros.h"

#include "emdevif/core/error_handler.hpp"

namespace emdevif {

/**
 * 默认的程序终止函数
 */
EMDEVIF_NO_RETURN static void defaultTerminateFunction() noexcept
{
    while (true) {
    }
}

/**
 * 终止程序的函数指针（编译期初始化为默认的终止函数）
 */
static constinit TerminateFunction terminateFunction = defaultTerminateFunction;

/**
 * 致命错误回调函数（编译期初始化为默认的回调函数）
 */
static constinit FatalHandlerCallBack fatalHandlerCallback = nullptr;
// ^-- 默认为 nullptr，表示不进行额外处理，直接调用 terminate

namespace detail {

constinit AssertFailedHandler assertFailedHandler = nullptr;

}  // namespace detail

EMDEVIF_NO_RETURN void terminate()
{
    terminateFunction();

    // 正常情况下，程序不应该运行到这里。但可能出现用户实现的 terminateFunction 返回的情况，
    // 因此增加进入死循环以确保不会返回，同时避免编译器警告。
    while (true) {
    }
}

void registerTerminateFunction(const TerminateFunction func) noexcept
{
    if (func != nullptr) {
        terminateFunction = func;
    }
}

EMDEVIF_NO_RETURN void fatalHandler(const char* file, const int line, const char* format, std::va_list args) noexcept
{
    if (fatalHandlerCallback != nullptr) {
        fatalHandlerCallback(file, line, format, args);
    }

    terminate();
}

EMDEVIF_NO_RETURN void fatalHandler(const char* file, const int line, const char* format, ...) noexcept
{
    std::va_list args;
    va_start(args, format);
    fatalHandler(file, line, format, args);
    va_end(args);
}

void registerFatalHandler(const FatalHandlerCallBack callback) noexcept
{
    if (callback != nullptr) {
        fatalHandlerCallback = callback;
    }
}

void registerAssertFailedHandler(const AssertFailedHandler handler) noexcept
{
    if (handler != nullptr) {
        detail::assertFailedHandler = handler;
    }
}

}  // namespace emdevif
