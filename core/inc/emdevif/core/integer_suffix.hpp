/**
 * @file integer_suffix.hpp
 * @brief 整型字面量后缀
 */

#pragma once
#ifndef EMDEVIF_CORE_INTEGER_SUFFIX_HPP
#define EMDEVIF_CORE_INTEGER_SUFFIX_HPP

#include <cstdint>

namespace emdevif::inline literals::inline integer_literals {

/** @brief 构造 uint8_t 字面量 */
constexpr std::uint8_t operator""_u8(const unsigned long long int v) noexcept
{
    return static_cast<std::uint8_t>(v);
}
/** @brief 构造 uint16_t 字面量 */
constexpr std::uint16_t operator""_u16(const unsigned long long int v) noexcept
{
    return static_cast<std::uint16_t>(v);
}
/** @brief 构造 uint32_t 字面量 */
constexpr std::uint32_t operator""_u32(const unsigned long long int v) noexcept
{
    return static_cast<std::uint32_t>(v);
}
/** @brief 构造 uint64_t 字面量 */
constexpr std::uint64_t operator""_u64(const unsigned long long int v) noexcept
{
    return static_cast<std::uint64_t>(v);
}

/** @brief 构造 int8_t 字面量 */
constexpr std::int8_t operator""_i8(const unsigned long long int v) noexcept
{
    return static_cast<std::int8_t>(v);
}
/** @brief 构造 int16_t 字面量 */
constexpr std::int16_t operator""_i16(const unsigned long long int v) noexcept
{
    return static_cast<std::int16_t>(v);
}
/** @brief 构造 int32_t 字面量 */
constexpr std::int32_t operator""_i32(const unsigned long long int v) noexcept
{
    return static_cast<std::int32_t>(v);
}
/** @brief 构造 int64_t 字面量 */
constexpr std::int64_t operator""_i64(const unsigned long long int v) noexcept
{
    return static_cast<std::int64_t>(v);
}

/** @brief 构造 ptrdiff_t 字面量 */
constexpr std::ptrdiff_t operator""_ptrdiff(const unsigned long long int v) noexcept
{
    return static_cast<std::ptrdiff_t>(v);
}

/** @brief 构造 size_t 字面量 */
constexpr std::size_t operator""_zu(const unsigned long long int v) noexcept
{
    return static_cast<std::size_t>(v);
}

}  // namespace emdevif::inline literals::inline integer_literals

#endif  // !EMDEVIF_CORE_INTEGER_SUFFIX_HPP
