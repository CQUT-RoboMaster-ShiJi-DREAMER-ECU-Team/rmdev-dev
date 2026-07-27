/**
 * @file decimal.hpp
 * @brief 十进制的计算
 */

#pragma once

#include <cstddef>
namespace rmdev {

/**
 * 获得给定位数的无符号整型的最大值
 * @param bits: 位数
 * @return 最大值
 */
constexpr std::size_t bitUintMax(const std::size_t bits) noexcept
{
    return (1U << bits) - 1U;
}

/**
 * 计算给定十进制整数的位数
 * @param integer: 待计算的十进制整数
 * @return 这个十进制整数的十进制位数（例如 123 是 3 位数）
 */
constexpr std::size_t integerBits(std::size_t integer) noexcept
{
    std::size_t count = 0U;

    while (integer != 0U) {
        integer /= 10;
        ++count;
    }

    return count;
}

}  // namespace rmdev
