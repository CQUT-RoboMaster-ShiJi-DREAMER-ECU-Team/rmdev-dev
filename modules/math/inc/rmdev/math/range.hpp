/**
 * @file range.hpp
 * @brief 范围相关的
 */

#pragma once

#include <algorithm>

#include "emdevif/core/concepts.hpp"

namespace rmdev {

/**
 * 限制值在给定范围内
 * @tparam Type 数据类型
 * @param[in, out] value 传入待限制的值，计算完成后通过引用返回计算结果
 * @param min 最小值
 * @param max 最大值
 * @return 限制计算完成后的值
 */
template<typename Type>
    requires emdevif::ArithmeticType<Type>
constexpr Type limitMinMax(Type& value, const Type min, const Type max) noexcept
{
    value = std::clamp(value, min, max);
    return value;
}

/**
 * 限制值在给定对称区间内
 * @tparam Type 数据类型
 * @param[in, out] value 传入待限制的值，计算完成后通过引用返回计算结果
 * @param range 对称区间半径
 * @return 限制计算完成后的值
 */
template<typename Type>
    requires emdevif::ArithmeticType<Type>
constexpr Type limitInRange(Type& value, Type range) noexcept
{
    value = limitMinMax(value, -range, range);
    return value;
}

}  // namespace rmdev
