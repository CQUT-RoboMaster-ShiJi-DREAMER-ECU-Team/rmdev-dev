/**
 * @file fixed_string.hpp
 * @brief 编译期固定大小字符串
 *
 * 提供编译期已知大小的固定容量字符串类型，可作为 NTTP（Non-Type Template Parameter）使用。
 *
 * 灵感来源于 P3094: std::basic_fixed_string
 */

#pragma once
#ifndef EMDEVIF_CORE_DATA_CONTAINER_FIXED_STRING_HPP
#define EMDEVIF_CORE_DATA_CONTAINER_FIXED_STRING_HPP

#include <cstddef>

#include <compare>
#include <concepts>
#include <iterator>
#include <limits>
#include <ranges>
#include <string_view>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"

namespace emdevif {

/**
 * @brief 编译期固定大小字符串
 *
 * 底层存储为 CharT[N + 1] 的字符数组（含空终止符），大小在编译期确定。
 * 支持 NTTP 传递、编译期拼接与比较。
 *
 * @tparam CharT 字符类型
 * @tparam N     字符串长度（不含空终止符）
 */
template<typename CharT, std::size_t N>
class BasicFixedString
{
public:
    static_assert(N <= std::numeric_limits<std::size_t>::max() - 1);

    /// 底层字符数组（公开以支持聚合初始化等场景）
    CharT data_[N + 1] = {};

    // 类型别名
    using value_type = CharT;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = const value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = std::reverse_iterator<const_iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // 构造与赋值 -------------------------------------------------------------

    /**
     * @brief 通过字符序列构造
     * @tparam Chars 字符形参包
     * @param  chars 字符序列，个数必须等于 N
     */
    template<std::same_as<CharT>... Chars>
        requires(sizeof...(Chars) == N) && (!std::is_pointer_v<Chars> && ...)
    constexpr explicit BasicFixedString(Chars... chars) noexcept : data_{chars..., CharT{}}
    {
    }

    /**
     * @brief 通过 C 风格字符串字面量隐式构造
     * @param txt 以空字符结尾的 C 风格字符串，长度必须为 N + 1
     */
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    consteval explicit(false) BasicFixedString(const CharT (&txt)[N + 1]) noexcept
    {
        EMDEVIF_ASSERT(txt[N] == CharT{});
        for (std::size_t i = 0; i < N; ++i) {
            data_[i] = txt[i];
        }
    }

    /**
     * @brief 通过迭代器对构造
     * @tparam It 输入迭代器类型
     * @tparam S  哨兵类型
     * @param  begin 起始迭代器
     * @param  end   结束哨兵，要求区间长度等于 N
     */
    template<std::input_iterator It, std::sentinel_for<It> S>
        requires std::same_as<std::iter_value_t<It>, CharT>
    constexpr BasicFixedString(It begin, S end)
    {
        EMDEVIF_ASSERT(std::distance(begin, end) == N);
        for (auto it = data_; begin != end; ++begin, ++it) {
            *it = *begin;
        }
    }

    constexpr BasicFixedString(const BasicFixedString&) noexcept = default;
    constexpr BasicFixedString& operator=(const BasicFixedString&) noexcept = default;

    // 迭代器支持 ---------------------------------------------------------------

    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return data();
    }
    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return data() + size();
    }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }
    [[nodiscard]] constexpr const_iterator cend() const noexcept
    {
        return end();
    }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    // 容量 -------------------------------------------------------------------

    static constexpr std::integral_constant<size_type, N> size{};
    static constexpr std::integral_constant<size_type, N> length{};
    static constexpr std::integral_constant<size_type, N> max_size{};
    static constexpr std::bool_constant<N == 0> empty{};

    // 元素访问 -----------------------------------------------------------------

    /**
     * @brief 下标访问
     * @param  pos 位置索引，必须小于 N
     * @return 对应位置字符的常引用
     */
    [[nodiscard]] constexpr const_reference operator[](size_type pos) const
    {
        EMDEVIF_ASSERT(pos < N);
        return data()[pos];
    }

    /**
     * @brief 访问首字符
     * @return 首字符的常引用
     */
    [[nodiscard]] constexpr const_reference front() const
    {
        EMDEVIF_ASSERT(!empty());
        return (*this)[0];
    }

    /**
     * @brief 访问末字符
     * @return 末字符的常引用
     */
    [[nodiscard]] constexpr const_reference back() const
    {
        EMDEVIF_ASSERT(!empty());
        return (*this)[N - 1];
    }

    // 修改器 ------------------------------------------------------------------

    /**
     * @brief 交换两个 BasicFixedString 的内容
     * @param s 另一个 BasicFixedString
     */
    constexpr void swap(BasicFixedString& s) noexcept
    {
        for (std::size_t i = 0; i <= N; ++i) {
            auto tmp = data_[i];
            data_[i] = s.data_[i];
            s.data_[i] = tmp;
        }
    }

    // 字符串操作 ----------------------------------------------------------------

    /**
     * @brief 获取 C 风格字符串指针
     * @return 指向以空字符结尾的底层字符数组的指针
     */
    [[nodiscard]] constexpr const_pointer c_str() const noexcept
    {
        return data();
    }

    /**
     * @brief 获取底层数据指针
     * @return 指向底层字符数组的指针
     */
    [[nodiscard]] constexpr const_pointer data() const noexcept
    {
        return static_cast<const_pointer>(data_);
    }

    /**
     * @brief 返回 string_view 视图
     * @return 覆盖所有 N 个字符的 basic_string_view
     */
    [[nodiscard]] constexpr std::basic_string_view<CharT> view() const noexcept
    {
        return std::basic_string_view<CharT>(cbegin(), cend());
    }

    /**
     * @brief 隐式转换为 basic_string_view
     */
    // NOLINTNEXTLINE(*-explicit-conversions, google-explicit-constructor)
    [[nodiscard]] constexpr explicit(false) operator std::basic_string_view<CharT>() const noexcept
    {
        return view();
    }

    // 拼接操作 ------------------------------------------------------------------

    /**
     * @brief 拼接两个 BasicFixedString
     * @tparam N2 右侧字符串的长度
     * @param  lhs 左侧字符串
     * @param  rhs 右侧字符串
     * @return 长度为 N + N2 的新字符串
     */
    template<std::size_t N2>
    [[nodiscard]] constexpr friend BasicFixedString<CharT, N + N2> operator+(
        const BasicFixedString& lhs,
        const BasicFixedString<CharT, N2>& rhs) noexcept
    {
        CharT txt[N + N2];
        CharT* it = txt;
        for (CharT ch : lhs) {
            *it = ch;
            ++it;
        }
        for (CharT ch : rhs) {
            *it = ch;
            ++it;
        }
        return BasicFixedString<CharT, N + N2>(txt, it);
    }

    /**
     * @brief 拼接 BasicFixedString 与单字符（右操作数）
     * @param  lhs 左侧字符串
     * @param  rhs 右侧单字符
     * @return 长度为 N + 1 的新字符串
     */
    [[nodiscard]] constexpr friend BasicFixedString<CharT, N + 1> operator+(const BasicFixedString& lhs,
                                                                            CharT rhs) noexcept
    {
        CharT txt[N + 1];
        CharT* it = txt;
        for (CharT ch : lhs) {
            *it = ch;
            ++it;
        }
        *it = rhs;
        ++it;
        return BasicFixedString<CharT, N + 1>(txt, it);
    }

    /**
     * @brief 拼接单字符与 BasicFixedString（左操作数）
     * @param  lhs 左侧单字符
     * @param  rhs 右侧字符串
     * @return 长度为 1 + N 的新字符串
     */
    [[nodiscard]] constexpr friend BasicFixedString<CharT, 1 + N> operator+(CharT lhs,
                                                                            const BasicFixedString& rhs) noexcept
    {
        CharT txt[1 + N];
        CharT* it = txt;
        *it = lhs;
        ++it;
        for (CharT ch : rhs) {
            *it = ch;
            ++it;
        }
        return BasicFixedString<CharT, 1 + N>(txt, it);
    }

    /**
     * @brief 拼接 BasicFixedString 与 C 风格字符串（右操作数）
     * @tparam N2 C 风格字符串的数组大小（含空终止符）
     * @param  lhs 左侧字符串
     * @param  rhs 右侧 C 风格字符串
     * @return 长度为 N + N2 - 1 的新字符串
     */
    template<std::size_t N2>
    [[nodiscard]] consteval friend BasicFixedString<CharT, N + N2 - 1> operator+(const BasicFixedString& lhs,
                                                                                 const CharT (&rhs)[N2]) noexcept
    {
        EMDEVIF_ASSERT(rhs[N2 - 1] == CharT{});
        CharT txt[N + N2];
        CharT* it = txt;
        for (CharT ch : lhs) {
            *it = ch;
            ++it;
        }
        for (CharT ch : rhs) {
            *it = ch;
            ++it;
        }
        return txt;
    }

    /**
     * @brief 拼接 C 风格字符串与 BasicFixedString（左操作数）
     * @tparam N1 C 风格字符串的数组大小（含空终止符）
     * @param  lhs 左侧 C 风格字符串
     * @param  rhs 右侧字符串
     * @return 长度为 N1 + N - 1 的新字符串
     */
    template<std::size_t N1>
    [[nodiscard]] consteval friend BasicFixedString<CharT, N1 + N - 1> operator+(const CharT (&lhs)[N1],
                                                                                 const BasicFixedString& rhs) noexcept
    {
        EMDEVIF_ASSERT(lhs[N1 - 1] == CharT{});
        CharT txt[N1 + N];
        CharT* it = txt;
        for (std::size_t i = 0; i != N1 - 1; ++i) {
            *it = lhs[i];
            ++it;
        }
        for (CharT ch : rhs) {
            *it = ch;
            ++it;
        }
        *it = CharT();
        ++it;
        return txt;
    }

    // 比较运算符 -----------------------------------------------------------------

    /**
     * @brief 相等比较（BasicFixedString 之间）
     */
    template<std::size_t N2>
    [[nodiscard]] friend constexpr bool operator==(const BasicFixedString& lhs, const BasicFixedString<CharT, N2>& rhs)
    {
        return lhs.view() == rhs.view();
    }

    /**
     * @brief 相等比较（BasicFixedString 与 C 风格字符串）
     */
    template<std::size_t N2>
    [[nodiscard]] friend consteval bool operator==(const BasicFixedString& lhs, const CharT (&rhs)[N2])
    {
        EMDEVIF_ASSERT(rhs[N2 - 1] == CharT{});
        return lhs.view() == std::basic_string_view<CharT>(std::cbegin(rhs), std::cend(rhs) - 1);
    }

    /**
     * @brief 三路比较（BasicFixedString 之间）
     */
    template<std::size_t N2>
    [[nodiscard]] friend constexpr auto operator<=>(const BasicFixedString& lhs, const BasicFixedString<CharT, N2>& rhs)
    {
        return lhs.view() <=> rhs.view();
    }

    /**
     * @brief 三路比较（BasicFixedString 与 C 风格字符串）
     */
    template<std::size_t N2>
    [[nodiscard]] friend consteval auto operator<=>(const BasicFixedString& lhs, const CharT (&rhs)[N2])
    {
        EMDEVIF_ASSERT(rhs[N2 - 1] == CharT{});
        return lhs.view() <=> std::basic_string_view<CharT>(std::cbegin(rhs), std::cend(rhs) - 1);
    }
};

// 推导指引 --------------------------------------------------------------------

template<typename CharT, std::same_as<CharT>... Rest>
BasicFixedString(CharT, Rest...) -> BasicFixedString<CharT, 1 + sizeof...(Rest)>;

template<typename CharT, std::size_t N>
BasicFixedString(const CharT (&str)[N]) -> BasicFixedString<CharT, N - 1>;

// swap 重载 -------------------------------------------------------------------

/**
 * @brief ADL swap 重载
 */
template<class CharT, std::size_t N>
constexpr void swap(BasicFixedString<CharT, N>& x, BasicFixedString<CharT, N>& y) noexcept
{
    x.swap(y);
}

// 类型别名 --------------------------------------------------------------------

/// char 特化的固定字符串
template<std::size_t N>
using FixedString = BasicFixedString<char, N>;

/// char8_t 特化的固定字符串
template<std::size_t N>
using FixedU8String = BasicFixedString<char8_t, N>;

/// char16_t 特化的固定字符串
template<std::size_t N>
using FixedU16String = BasicFixedString<char16_t, N>;

/// char32_t 特化的固定字符串
template<std::size_t N>
using FixedU32String = BasicFixedString<char32_t, N>;

/// wchar_t 特化的固定字符串
template<std::size_t N>
using FixedWString = BasicFixedString<wchar_t, N>;

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_DATA_CONTAINER_FIXED_STRING_HPP