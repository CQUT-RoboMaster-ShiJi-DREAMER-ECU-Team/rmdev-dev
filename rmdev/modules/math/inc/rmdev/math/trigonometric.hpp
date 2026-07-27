/**
 * @file trigonometric.hpp
 * @brief 三角学相关函数 - 通用 (std::) 实现
 */

#pragma once

#include <cmath>

#include <tuple>
#include <utility>

#include "emdevif/core/concepts.hpp"
#include "rmdev/math/const_value.hpp"

namespace rmdev {

template<emdevif::ArithmeticType Type>
constexpr float angleToRad(const Type angle) noexcept
{
    return angle * Type(num::pi) / Type(180);
}

template<emdevif::ArithmeticType Type>
constexpr float radToAngle(const Type rad) noexcept
{
    return rad * Type(180) / Type(num::pi);
}

template<emdevif::ArithmeticType Type>
Type sin(const Type x)
{
    return std::sin(x);
}

template<emdevif::ArithmeticType Type>
Type cos(const Type x) noexcept
{
    return std::cos(x);
}

template<emdevif::ArithmeticType Type>
std::pair<Type, Type> sin_cos(const Type x) noexcept
{
    return {std::sin(x), std::cos(x)};
}

template<emdevif::ArithmeticType Type>
void sin_cos(const Type x, Type& sin_out, Type& cos_out) noexcept
{
    sin_out = std::sin(x);
    cos_out = std::cos(x);
}

}  // namespace rmdev

#ifdef RMDEV_MATH_USE_CMSIS_DSP
#include "rmdev/math/trigonometric_cmsis_impl.hpp"
#endif
