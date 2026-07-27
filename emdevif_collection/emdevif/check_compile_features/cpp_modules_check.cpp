/**
 * @file cpp_modules_check.cpp
 * @brief 检查是否支持模块功能
 */

#include <version>

#if (defined(__cpp_modules))
// This compiler supports C++ modules
#else
#error "This compiler does not support C++ modules."
#endif

#if (__cpp_modules >= 201907L)
// This compiler supports C++20 modules fully
#else
#warning "This compiler does not support C++20 modules fully."
#endif
