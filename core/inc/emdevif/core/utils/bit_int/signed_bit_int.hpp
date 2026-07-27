/**
 * @file signed_bit_int.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_CORE_UTILS_BIT_INT_SIGNED_BIT_INT_HPP
#define EMDEVIF_CORE_UTILS_BIT_INT_SIGNED_BIT_INT_HPP

#include "emdevif/core/utils/bit_int/basic_config.hpp"

#include <cstdint>

#include <bit>
#include <compare>
#include <concepts>
#include <iostream>
#include <numeric>

#include "emdevif/core/concepts.hpp"
#include "emdevif/core/utils/bit_int/detail/exception_config.hpp"

namespace emdevif {

/**
 * @brief 有符号指定位宽整数类
 * @tparam bits 位宽，必须满足 ValidBitIntWidth 概念
 */
template<BitsType_t bits>
    requires ValidBitIntWidth<bits>
class BitInt
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

    /**
     * @brief 有符号整数类型，与 RealType 对应
     */
    using SignedType =
        std::conditional_t<(bits <= 8U), int8_t,
            std::conditional_t<(bits > 8U && bits <= 16U), int16_t,
                std::conditional_t<(bits > 16U && bits <= 32U), int32_t,
                    int64_t
                >
            >
        >;

    /* clang-format on */

    static_assert(sizeof(SignedType) == sizeof(RealType));
    static_assert(detail::bitsOf<RealType>() != 0);

    /**
     * @brief 将有符号值截断到指定位宽的无符号存储值
     * @param v 有符号输入值
     * @return 截断后的无符号值
     */
    static constexpr RealType truncateToReal(const SignedType v) noexcept
    {
        if constexpr (bits == detail::bitsOf<RealType>()) {
            return std::bit_cast<RealType>(v);
        }
        else {
            return static_cast<RealType>(v) & ((RealType(1) << bits) - 1);
        }
    }

    /**
     * @brief 将无符号存储值符号扩展为有符号类型
     * @param v 无符号存储值
     * @return 符号扩展后的有符号值
     */
    static constexpr SignedType transToSigned(const RealType v) noexcept
    {
        if constexpr (bits == detail::bitsOf<RealType>()) {
            return std::bit_cast<SignedType>(v);
        }
        else {
            constexpr auto shift = detail::bitsOf<RealType>() - bits;
            return (static_cast<SignedType>(v << shift)) >> shift;
        }
    }

    /**
     * @brief 生成指定位数的置位掩码
     * @param value_bits 需要置位的位数
     * @return 掩码值
     */
    static constexpr SignedType bitsSetValue(const BitsType_t value_bits) noexcept
    {
        SignedType result = 0U;

        for (BitsType_t i = 0U; i < value_bits; ++i) {
            result |= (static_cast<SignedType>(1) << i);
        }

        return result;
    }

public:
    static constexpr SignedType max() noexcept
    {
        return bitsSetValue(bits - 1);
    }

    static constexpr SignedType min() noexcept
    {
        if constexpr (bits == 64) {
            return std::numeric_limits<int64_t>::min();
        }
        else {
            return -static_cast<int64_t>(RealType(1) << RealType(bits - 1));
        }
    }

    constexpr BitInt() noexcept = default;

    template<std::integral OtherType>  // NOLINTNEXTLINE(*-explicit-constructor)
    constexpr BitInt(const OtherType other) noexcept : value(truncateToReal(other))
    {
    }

    template<std::integral OtherType>
    constexpr BitInt& operator=(const OtherType other) noexcept
    {
        value = truncateToReal(other);

        return *this;
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>  // NOLINTNEXTLINE(*-explicit-constructor)
    constexpr BitInt(const BitInt<other_bits>& other) noexcept : value(truncateToReal(other))
    {
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr BitInt& operator=(const BitInt<other_bits>& other) noexcept
    {
        if (static_cast<void*>(this) == static_cast<const void*>(&other)) {
            return *this;
        }

        value = truncateToReal(static_cast<SignedType>(other));

        return *this;
    }

    // ---------------------------- 一元 ---------------------------------
    // =========================== ~

    constexpr SignedType operator~() const noexcept
    {
        const SignedType tmp = transToSigned(value);

        return ~tmp;
    }

    // =========================== !

    constexpr bool operator!() const noexcept
    {
        return !static_cast<bool>(transToSigned(value));
    }

    // =========================== +/-

    constexpr SignedType operator+() const noexcept
    {
        return SignedType(*this);
    }
    constexpr SignedType operator-() const noexcept
    {
        return -transToSigned(value);
    }

    // ---------------------------- 二元 ---------------------------------
    // =========================== +

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, SignedType>)
    friend constexpr SignedType operator+(const OtherType& lhs, const BitInt& rhs) noexcept
    {
        return static_cast<SignedType>(lhs) + transToSigned(rhs.value);
    }

    // =========================== -

    template<ArithmeticType OtherType>
    friend constexpr SignedType operator-(const OtherType& lhs, const BitInt& rhs) noexcept
    {
        return (static_cast<SignedType>(lhs) - transToSigned(rhs.value));
    }

    // =========================== *

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, SignedType>)
    friend constexpr SignedType operator*(const OtherType& lhs, const BitInt& rhs) noexcept
    {
        return (static_cast<SignedType>(lhs) * transToSigned(rhs.value));
    }

    // =========================== /

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, SignedType>)
    friend constexpr SignedType operator/(const OtherType& lhs, const BitInt& rhs) EMDEVIF_UTIL_BITINT_NOEXCEPT
    {
        const auto rhs_signed = transToSigned(rhs.value);

#if (defined(EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS) && EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS)
        if (rhs_signed == 0) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
#endif

        return (static_cast<SignedType>(lhs) / rhs_signed);
    }

    // =========================== %

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, SignedType>)
    friend constexpr SignedType operator%(const OtherType& lhs, const BitInt& rhs) EMDEVIF_UTIL_BITINT_NOEXCEPT
    {
        const auto rhs_signed = transToSigned(rhs.value);

#if (defined(EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS) && EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS)
        if (rhs_signed == 0) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
#endif

        return (static_cast<SignedType>(lhs) % rhs_signed);
    }

    // =========================== &

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, SignedType>)
    friend constexpr SignedType operator&(const OtherType& lhs, const BitInt& rhs) noexcept
    {
        return (static_cast<SignedType>(lhs) & transToSigned(rhs.value));
    }

    // =========================== |

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, SignedType>)
    friend constexpr SignedType operator|(const OtherType& lhs, const BitInt& rhs) noexcept
    {
        return (static_cast<SignedType>(lhs) | transToSigned(rhs.value));
    }

    // =========================== ^

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, SignedType>)
    friend constexpr SignedType operator^(const OtherType& lhs, const BitInt& rhs) noexcept
    {
        return (static_cast<SignedType>(lhs) ^ transToSigned(rhs.value));
    }

    // =========================== <<

    template<std::integral OtherType>
    constexpr SignedType operator<<(const OtherType shift) const noexcept
    {
        return (transToSigned(value) << shift);
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr SignedType operator<<(const BitInt<other_bits> shift) const noexcept
    {
        return (transToSigned(value) << SignedType(shift));
    }

    // =========================== >>

    template<std::integral OtherType>
    constexpr SignedType operator>>(const OtherType shift) const noexcept
    {
        return (transToSigned(value) >> shift);
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr SignedType operator>>(const BitInt<other_bits> shift) const noexcept
    {
        return (transToSigned(value) >> SignedType(shift));
    }

    // ---------------------------- 复合赋值 -------------------------------
    // =========================== +=

    template<typename OtherType>
    constexpr BitInt& operator+=(const OtherType& other) noexcept
    {
        return *this = *this + other;
    }

    // =========================== -=

    template<typename OtherType>
    constexpr BitInt& operator-=(const OtherType& other) noexcept
    {
        return *this = *this - other;
    }

    // =========================== *=

    template<typename OtherType>
    constexpr BitInt& operator*=(const OtherType& other) noexcept
    {
        return *this = *this * other;
    }

    // =========================== /=

    template<typename OtherType>
    constexpr BitInt& operator/=(const OtherType& other) EMDEVIF_UTIL_BITINT_NOEXCEPT
    {
        const auto other_signed = static_cast<SignedType>(other);

#if (defined(EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS) && EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS)
        if (other_signed == 0) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
#endif

        return *this = *this / other_signed;
    }

    // =========================== %=

    template<typename OtherType>
    constexpr BitInt& operator%=(const OtherType& other) EMDEVIF_UTIL_BITINT_NOEXCEPT
    {
        const auto other_signed = static_cast<SignedType>(other);

#if (defined(EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS) && EMDEVIF_UTIL_BIT_INT_USE_EXCEPTIONS)
        if (other_signed == 0) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
#endif

        return *this = *this % other_signed;
    }

    // =========================== &=

    template<typename OtherType>
    constexpr BitInt& operator&=(const OtherType& other) noexcept
    {
        return *this = *this & other;
    }

    // =========================== |=

    template<typename OtherType>
    constexpr BitInt& operator|=(const OtherType& other) noexcept
    {
        return *this = *this | other;
    }

    // =========================== ^=

    template<typename OtherType>
    constexpr BitInt& operator^=(const OtherType& other) noexcept
    {
        return *this = *this ^ other;
    }

    // =========================== <<=

    template<std::integral OtherType>
    constexpr BitInt& operator<<=(const OtherType shift) const noexcept
    {
        return *this = *this << shift;
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr BitInt& operator<<=(const BitInt<other_bits> shift) const noexcept
    {
        return *this = *this << shift;
    }

    // =========================== >>=

    template<std::integral OtherType>
    constexpr BitInt& operator>>=(const OtherType shift) const noexcept
    {
        return *this = *this >> shift;
    }

    template<BitsType_t other_bits>
        requires ValidBitIntWidth<other_bits>
    constexpr BitInt& operator>>=(const BitInt<other_bits> shift) const noexcept
    {
        return *this = *this >> shift;
    }

    // ---------------------------- 自增/自减 ----------------------------
    // =========================== ++

    constexpr SignedType operator++() noexcept
    {
        return *this += 1;
    }
    constexpr SignedType operator++(int) noexcept
    {
        const auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // =========================== --

    constexpr SignedType operator--() noexcept
    {
        return *this -= 1;
    }
    constexpr SignedType operator--(int) noexcept
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    // ---------------------------- 比较 ----------------------------

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, SignedType>)
    friend constexpr bool operator==(const OtherType& lhs, const BitInt& rhs) noexcept
    {
        return (static_cast<SignedType>(lhs) == transToSigned(rhs.value));
    }

    template<typename OtherType>
        requires(std::is_convertible_v<OtherType, SignedType>)
    friend constexpr std::strong_ordering operator<=>(const OtherType& lhs, const BitInt& rhs) noexcept
    {
        return static_cast<SignedType>(lhs) - transToSigned(rhs.value);
    }

    // --------------------------- stream ------------------------------

    inline friend std::ostream& operator<<(std::ostream& os, const BitInt& bit_int)
    {
        os << static_cast<SignedType>(bit_int);
        return os;
    }

    // end operators ---------------------------------------------------

    template<std::integral OtherType>
    operator OtherType() const noexcept  // NOLINT(*-explicit-constructor)
    {
        return static_cast<OtherType>(transToSigned(value));
    }

    ~BitInt() noexcept = default;

private:
    RealType value;
};

/**
 * @brief BitInt<0> 的特化版本，始终为零值
 */
template<>
class BitInt<0>
{
public:
    static constexpr int8_t max() noexcept
    {
        return 0;
    }

    static constexpr int8_t min() noexcept
    {
        return 0;
    }

    inline friend std::ostream& operator<<(std::ostream& os, const BitInt&)
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
    const int8_t value{0};
};

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_UTILS_BIT_INT_SIGNED_BIT_INT_HPP
