/**
 * @file unsigned_bit_int.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_CORE_UTILS_BIT_INT_UNSIGNED_BIT_INT_HPP
#define EMDEVIF_CORE_UTILS_BIT_INT_UNSIGNED_BIT_INT_HPP

#include "emdevif/core/utils/bit_int/basic_config.hpp"

#include <cstdint>

#include <compare>
#include <concepts>
#include <iostream>
#include <type_traits>

#include "emdevif/core/concepts.hpp"
#include "emdevif/core/utils/bit_int/detail/exception_config.hpp"

namespace emdevif {

/**
 * @brief 无符号指定位宽整数类
 * @tparam bits 位宽，必须满足 ValidBitIntWidth 概念
 */
template<BitsType_t bits>
    requires ValidBitIntWidth<bits>
class UBitInt
{
protected:  // for testing
    /* clang-format off */

    /**
     * @brief 底层存储的无符号整数类型，根据位宽自动选择 uint8/16/32/64
     */
    using RealType =
        std::conditional_t<(bits <= 8U), uint8_t,
            std::conditional_t<(bits > 8U && bits <= 16U), uint16_t,
                std::conditional_t<(bits > 16U && bits <= 32U), uint32_t,
                    uint64_t
                >
            >
        >;

    /* clang-format on */

    static_assert(detail::bitsOf<RealType>() != 0);

    /**
     * @brief 将值截断到指定位宽，处理溢出
     * @param value 输入值
     * @return 截断后的值
     */
    static constexpr RealType overflowCast(const RealType value) noexcept
    {
        if constexpr (bits == detail::bitsOf<RealType>()) {
            return value;
        }
        else {
            return value & ((RealType(1) << RealType(bits)) - RealType(1));
        }
    }

public:
    static constexpr RealType max() noexcept
    {
        if constexpr (bits == detail::bitsOf<RealType>()) {
            return std::numeric_limits<RealType>::max();
        }
        else {
            return ((RealType(1) << RealType(bits)) - RealType(1));
        }
    }

    static constexpr RealType min() noexcept
    {
        return 0U;
    }

    constexpr UBitInt() noexcept = default;

    template<std::integral OtherType>  // NOLINTNEXTLINE(*-explicit-constructor)
    constexpr UBitInt(const OtherType other) noexcept : value(overflowCast(static_cast<RealType>(other)))
    {
    }

    template<std::integral OtherType>
    constexpr UBitInt& operator=(const OtherType other) noexcept
    {
        value = overflowCast(static_cast<RealType>(other));

        return *this;
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>  // NOLINTNEXTLINE(*-explicit-constructor)
    constexpr UBitInt(const UBitInt<other_bits>& other) noexcept : value(overflowCast(static_cast<RealType>(other)))
    {
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr UBitInt& operator=(const UBitInt<other_bits>& other) noexcept
    {
        value = overflowCast(static_cast<RealType>(other));

        return *this;
    }

    // ---------------------------- 一元 ---------------------------------
    // =========================== ~

    constexpr RealType operator~() const noexcept
    {
        return overflowCast(~value);
    }

    // =========================== !

    constexpr bool operator!() const noexcept
    {
        return !static_cast<bool>(overflowCast(value));
    }

    // =========================== +/-

    constexpr RealType operator+() const noexcept
    {
        return RealType(*this);
    }
    constexpr RealType operator-() const noexcept
    {
        return overflowCast(-value);
    }

    // ---------------------------- 二元 ---------------------------------
    // =========================== +

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr RealType operator+(const OtherType& lhs, const UBitInt& rhs) noexcept
    {
        return overflowCast(static_cast<RealType>(lhs) + rhs.value);
    }

    // =========================== -

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr RealType operator-(const OtherType& lhs, const UBitInt& rhs) noexcept
    {
        return overflowCast(static_cast<RealType>(lhs) - rhs.value);
    }

    // =========================== *

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr RealType operator*(const OtherType& lhs, const UBitInt& rhs) noexcept
    {
        return overflowCast(static_cast<RealType>(lhs) * rhs.value);
    }

    // =========================== /

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr RealType operator/(const OtherType& lhs, const UBitInt& rhs) EMDEVIF_UTIL_BITINT_NOEXCEPT
    {
        const auto rhs_val = static_cast<RealType>(rhs.value);

#if (defined(EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS) && EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS)
        if (rhs_val == 0) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
#endif

        return overflowCast(static_cast<RealType>(lhs) / rhs_val);
    }

    // =========================== %

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr RealType operator%(const OtherType& lhs, const UBitInt& rhs) EMDEVIF_UTIL_BITINT_NOEXCEPT
    {
        const auto rhs_val = static_cast<RealType>(rhs.value);

#if (defined(EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS) && EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS)
        if (rhs_val == 0) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
#endif

        return overflowCast(static_cast<RealType>(lhs) % rhs_val);
    }

    // =========================== &

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr RealType operator&(const OtherType& lhs, const UBitInt& rhs) noexcept
    {
        return overflowCast(static_cast<RealType>(lhs) & rhs.value);
    }

    // =========================== |

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr RealType operator|(const OtherType& lhs, const UBitInt& rhs) noexcept
    {
        return overflowCast(static_cast<RealType>(lhs) | rhs.value);
    }

    // =========================== ^

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr RealType operator^(const OtherType& lhs, const UBitInt& rhs) noexcept
    {
        return overflowCast(static_cast<RealType>(lhs) ^ rhs.value);
    }

    // =========================== <<

    template<std::integral OtherType>
    constexpr RealType operator<<(const OtherType shift) const noexcept
    {
        return overflowCast(value << shift);
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr RealType operator<<(const UBitInt<other_bits> shift) const noexcept
    {
        return overflowCast(value << RealType(shift));
    }

    // =========================== >>

    template<std::integral OtherType>
    constexpr RealType operator>>(const OtherType shift) const noexcept
    {
        return overflowCast(value >> shift);
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr RealType operator>>(const UBitInt<other_bits> shift) const noexcept
    {
        return overflowCast(value >> RealType(shift));
    }

    // ---------------------------- 复合赋值 -------------------------------
    // =========================== +=

    template<typename OtherType>
    constexpr UBitInt& operator+=(const OtherType& other) noexcept
    {
        return *this = *this + other;
    }

    // =========================== -=

    template<typename OtherType>
    constexpr UBitInt& operator-=(const OtherType& other) noexcept
    {
        return *this = *this - other;
    }

    // =========================== *=

    template<typename OtherType>
    constexpr UBitInt& operator*=(const OtherType& other) noexcept
    {
        return *this = *this * other;
    }

    // =========================== /=

    template<typename OtherType>
    constexpr UBitInt& operator/=(const OtherType& other) EMDEVIF_UTIL_BITINT_NOEXCEPT
    {
#if (defined(EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS) && EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS)
        RealType res;

        try {
            res = *this / other;
        }
        catch (std::invalid_argument&) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }

        return *this = res;
#else
        return *this = *this / other;
#endif
    }

    // =========================== %=

    template<typename OtherType>
    constexpr UBitInt& operator%=(const OtherType& other) EMDEVIF_UTIL_BITINT_NOEXCEPT
    {
#if (defined(EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS) && EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS)
        RealType res;

        try {
            res = *this % other;
        }
        catch (std::invalid_argument&) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }

        return *this = res;
#else
        return *this = *this % other;
#endif
    }

    // =========================== &=

    template<typename OtherType>
    constexpr UBitInt& operator&=(const OtherType& other) noexcept
    {
        return *this = *this & other;
    }

    // =========================== |=

    template<typename OtherType>
    constexpr UBitInt& operator|=(const OtherType& other) noexcept
    {
        return *this = *this | other;
    }

    // =========================== ^=

    template<typename OtherType>
    constexpr UBitInt& operator^=(const OtherType& other) noexcept
    {
        return *this = *this ^ other;
    }

    // =========================== <<=

    template<std::integral OtherType>
    constexpr UBitInt& operator<<=(const OtherType shift) const noexcept
    {
        return *this = *this << shift;
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr UBitInt& operator<<=(const UBitInt<other_bits> shift) const noexcept
    {
        return *this = *this << shift;
    }

    // =========================== >>=

    template<std::integral OtherType>
    constexpr UBitInt& operator>>=(const OtherType shift) const noexcept
    {
        return *this = *this >> shift;
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr UBitInt& operator>>=(const UBitInt<other_bits> shift) const noexcept
    {
        return *this = *this >> shift;
    }

    // ---------------------------- 自增/自减 ----------------------------
    // =========================== ++

    constexpr RealType operator++() noexcept
    {
        return *this += 1;
    }
    constexpr RealType operator++(int) noexcept
    {
        const auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // =========================== --

    constexpr RealType operator--() noexcept
    {
        return *this -= 1;
    }
    constexpr RealType operator--(int) noexcept
    {
        const auto tmp = *this;
        --(*this);
        return tmp;
    }

    // ---------------------------- 比较 ----------------------------

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr bool operator==(const OtherType& lhs, const UBitInt& rhs) noexcept
    {
        return (static_cast<RealType>(lhs) == rhs.value);
    }

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, RealType>)
    friend constexpr std::strong_ordering operator<=>(const OtherType& lhs, const UBitInt& rhs) noexcept
    {
        const auto l_value = static_cast<RealType>(lhs);
        const auto r_value = rhs.value;

        if (l_value < r_value) {
            return std::strong_ordering::less;
        }
        if (l_value > r_value) {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }

    // --------------------------- stream ------------------------------

    inline friend std::ostream& operator<<(std::ostream& os, const UBitInt& bit_int)
    {
        os << static_cast<RealType>(bit_int);
        return os;
    }

    // end operators ---------------------------------------------------

    template<std::integral OtherType>
    operator OtherType() const noexcept  // NOLINT(*-explicit-constructor)
    {
        return static_cast<OtherType>(overflowCast(value));
    }

    constexpr ~UBitInt() noexcept = default;

private:
    RealType value;
};

/**
 * @brief UBitInt<0> 的特化版本，始终为零值
 */
template<>
class UBitInt<0>
{
public:
    static constexpr uint8_t max() noexcept
    {
        return 0;
    }

    static constexpr uint8_t min() noexcept
    {
        return 0;
    }

    inline friend std::ostream& operator<<(std::ostream& os, const UBitInt&)
    {
        os << 0;
        return os;
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    template<std::integral Type>
    operator Type() const noexcept  // NOLINT(*-explicit-constructor)
    {
        return static_cast<Type>(value);
    }

private:
    const uint8_t value{0U};
};

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_UTILS_BIT_INT_UNSIGNED_BIT_INT_HPP
