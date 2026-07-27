/**
 * @file concepts.hpp
 * @brief 概念
 */

#pragma once
#ifndef EMDEVIF_CORE_CONCEPTS_HPP
#define EMDEVIF_CORE_CONCEPTS_HPP

#include <type_traits>
#include <concepts>
#include <compare>
#include <limits>

namespace emdevif {

/**
 * 有界的类型概念
 */
template<typename T>
concept HaveLimitType = ((std::is_arithmetic_v<T> || std::same_as<std::remove_cvref_t<T>, bool>) && requires {
                            std::numeric_limits<T>::max();
                            std::numeric_limits<T>::min();
                        }) || requires {
    T::max();
    T::min();
} || requires(T v, const T cv) {
    v.max();
    v.min();

    cv.max();
    cv.min();
};

/**
 * 可计算类型概念
 * @attention 满足 emdevif::ArithmeticType 概念的类型区别于满足于 std::is_arithmetic 类型特征的类型，
 * 前者表示满足于算术运算的类型，可以是用户自定义的类型（如复数、矩阵等）；后者只能是内置的类型（整型与浮点）。
 * @tparam Type 数据类型
 */
template<typename Type>
concept ArithmeticType = HaveLimitType<Type> && requires(Type lhs, Type rhs) {
    lhs + rhs;
    lhs - rhs;
    /* clang-format off */
    lhs * rhs;
    /* clang-format on */
    lhs / rhs;
    lhs = rhs;
    lhs <=> rhs;
};

/**
 * 浮点类型概念
 * @tparam Type 数据类型
 */
template<typename Type>
concept FloatingPointType = std::is_floating_point_v<Type>;

/**
 * 指针类型概念
 * @tparam Type 类型
 */
template<typename Type>
concept PointerType = std::is_pointer_v<Type>;

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_CONCEPTS_HPP
