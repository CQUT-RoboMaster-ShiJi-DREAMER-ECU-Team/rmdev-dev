/**
 * @file endian.hpp
 * @brief 端序
 */

#pragma once
#ifndef EMDEVIF_CORE_ENDIAN_HPP
#define EMDEVIF_CORE_ENDIAN_HPP

#include <cstdint>

#include <algorithm>
#include <array>
#include <bit>
#include <concepts>

namespace emdevif {

namespace detail {

/**
 * 反转任意类型的端序
 * @tparam T 待反转端序的类型
 * @param value 待反转端序的变量值
 * @return 反转后的值
 */
template<typename T>
    requires(std::has_unique_object_representations_v<T>)
constexpr T byteSwapGeneric(T value) noexcept
{
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    std::ranges::reverse(value_representation);
    return std::bit_cast<T>(value_representation);
}

/**
 * 反转整型类型的端序
 * @tparam T 待反转端序的整型
 * @param v 待反转端序的整型变量值
 * @return 反转后的值
 */
template<std::integral T>
constexpr T byteSwapIntegral(const T v) noexcept
{
#ifdef __cpp_lib_byteswap
    return std::byteswap(v);
#else

    if constexpr (sizeof(T) == 1) {
        return v;
    }
    else if constexpr (sizeof(T) == 2) {
        return ((((v) >> 8) & 0xFFU) | (((v) & 0xFFU) << 8));
    }
    else if constexpr (sizeof(T) == 4) {
        return ((((v) & 0xFF000000U) >> 24) | (((v) & 0x00FF0000U) >> 8) | (((v) & 0x0000FF00U) << 8) |
                (((v) & 0x000000FFU) << 24));
    }
    else if constexpr (sizeof(T) == 8) {
        return ((((v) & 0xFF00000000000000ULL) >> 56) | (((v) & 0x00FF000000000000ULL) >> 40) |
                (((v) & 0x0000FF0000000000ULL) >> 24) | (((v) & 0x000000FF00000000ULL) >> 8) |
                (((v) & 0x00000000FF000000ULL) << 8) | (((v) & 0x0000000000FF0000ULL) << 24) |
                (((v) & 0x000000000000FF00ULL) << 40) | (((v) & 0x00000000000000FFULL) << 56));
    }
    else {
        return byteSwapGeneric(v);
    }

#endif
}

}  // namespace detail

/**
 * 反转端序
 * @note 会根据变量的类型与大小自动选择不同的方式反转
 * @tparam T 待反转端序的类型
 * @param v 待反转端序的变量值
 * @return 反转后的值
 */
template<typename T, typename U = std::remove_cvref_t<T>>
constexpr U byteSwap(const T v) noexcept
{
    if constexpr (sizeof(U) == 1) {
        return v;
    }
    else if constexpr (constexpr std::array integral_support_size_list{2, 4, 8};
                       std::ranges::find(integral_support_size_list, sizeof(U))) {
        if constexpr (!std::is_integral_v<U>) {
            constexpr auto size_of_U = sizeof(U);
            /* clang-format off */
            using ConvertIntegral =
                std::conditional_t<(size_of_U <= 1U), uint8_t,
                    std::conditional_t<(size_of_U >= 2U && size_of_U <= 3U), uint16_t,
                        std::conditional_t<(size_of_U >= 4U && size_of_U <= 7U), uint32_t,
                            uint64_t
                        >
                    >
                >;
            /* clang-format on */

            auto u = std::bit_cast<ConvertIntegral>(v);
            u = detail::byteSwapIntegral(u);
            return std::bit_cast<U>(u);
        }
        else {
            return detail::byteSwapIntegral(v);
        }
    }
    else {
        return detail::byteSwapGeneric(v);
    }
}

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_ENDIAN_HPP
