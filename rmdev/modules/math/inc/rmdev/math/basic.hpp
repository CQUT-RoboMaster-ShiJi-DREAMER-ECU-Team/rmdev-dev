/**
 * @file basic.hpp
 * @brief 基本
 */

#pragma once

#include <cmath>

#include "rmdev/math/const_value.hpp"

#include "emdevif/core/concepts.hpp"

namespace rmdev {

/**
 * 符号函数
 * @tparam Type: 数字类型（应当为整型数字）
 * @param n: 待判断符号的数字
 * @retval 0 n 等于零
 * @retval 1 n 大于零
 * @retval -1 n 小于零
 */
template<typename Type>
    requires emdevif::ArithmeticType<Type>
constexpr Type sgn(const Type n) noexcept
{
    if (n > 0) {
        return 1;
    }
    if (n < 0) {
        return -1;
    }

    return 0;
}

/**
 * 符号函数 - @c float 类型特例
 * @param n: 待判断符号的数字
 * @retval 1.0f n 大于或等于零
 * @retval -1.0f n 小于零
 */
template<>
constexpr float sgn<float>(const float n) noexcept
{
    return ((n) >= 0.0f ? (1.0f) : (-1.0f));
}

/**
 * 符号函数 - @c double 类型特例
 * @param n: 待判断符号的数字
 * @retval 1.0 n 大于或等于零
 * @retval -1.0 n 小于零
 */
template<>
constexpr double sgn<double>(const double n) noexcept
{
    return ((n) >= 0.0 ? (1.0) : (-1.0));
}

/**
 * 弱等于
 * @tparam Type 类型
 * @param a 第一个数
 * @param b 第二个数
 * @param error 允许的误差（a、b 之差的绝对值小于这个值即认为相等）
 * @return a、b 是否相等
 */
template<typename Type>
    requires emdevif::ArithmeticType<Type>
constexpr bool weakEqu(const Type a, const Type b, Type error) noexcept
{
    return std::abs(a - b) <= error;
}

/**
 * 弱等于
 * @note 如果是浮点数，则使用默认的误差值进行弱等于比较；否则表示严格等于
 * @tparam Type 类型
 * @param a 第一个数
 * @param b 第二个数
 * @return a、b 是否相等
 */
template<typename Type>
    requires emdevif::ArithmeticType<Type>
constexpr bool weakEqu(const Type a, const Type b) noexcept
{
    if constexpr (std::is_floating_point_v<Type>) {
        return weakEqu(a, b, Type(num::detail::float_equ_default_error_value));
    }

    return (a == b);
}

}  // namespace rmdev
