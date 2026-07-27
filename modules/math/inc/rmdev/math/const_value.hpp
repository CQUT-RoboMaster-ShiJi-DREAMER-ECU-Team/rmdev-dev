/**
 * @file const_value.hpp
 * @brief 数学常用常量
 */

#pragma once

#include <numbers>

#ifndef RMDEV_FLOAT_EQU_DEFAULT_ERROR_VALUE
/// 默认的浮点数等于比较误差值（当两个浮点数的绝对值之差小于这个值时，认为二者相等）
/// @note 这个值可以通过编译参数定义宏 RMDEV_FLOAT_EQU_DEFAULT_ERROR_VALUE 来修改。
///       如果没有定义，则使用默认值 0.001f
#define RMDEV_FLOAT_EQU_DEFAULT_ERROR_VALUE 0.001f
#endif
namespace rmdev::num {

namespace detail {

/// rmdev 中，默认的浮点数等于比较误差值
inline constexpr auto float_equ_default_error_value = static_cast<double>(RMDEV_FLOAT_EQU_DEFAULT_ERROR_VALUE);

}  // namespace detail

inline constexpr auto e = std::numbers::e;                   ///< e
inline constexpr auto log2_e = std::numbers::log2e;          ///< log2(e)
inline constexpr auto lge = std::numbers::log10e;            ///< log10(e)
inline constexpr auto ln2 = std::numbers::ln2;               ///< ln(2)
inline constexpr auto ln10 = std::numbers::ln10;             ///< ln(10)
inline constexpr auto pi = std::numbers::pi;                 ///< pi
inline constexpr auto pi_inv2 = 1.57079632679489661923;      ///< pi/2
inline constexpr auto pi_inv4 = 0.785398163397448309616;     ///< pi/4
inline constexpr auto invPi = std::numbers::inv_pi;          ///< 1/pi
inline constexpr auto invPi_2 = 0.636619772367581343076;     ///< 2/pi
inline constexpr auto invSqrtPi_2 = 1.12837916709551257390;  ///< 2/sqrt(pi)
inline constexpr auto sqrt2 = std::numbers::sqrt2;           ///< sqrt(2)
inline constexpr auto invSqrt2 = 0.707106781186547524401;    ///< 1/sqrt(2)

}  // namespace rmdev::num
