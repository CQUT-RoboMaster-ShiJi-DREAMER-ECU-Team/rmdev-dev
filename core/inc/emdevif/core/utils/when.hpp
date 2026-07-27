/**
 * @file when.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_CORE_UTILS_WHEN_HPP
#define EMDEVIF_CORE_UTILS_WHEN_HPP

#include <concepts>
#include <type_traits>
#include <utility>

namespace emdevif {

/// 默认标签类型
struct default_tag_t {
};

/// 默认标签，用于 when 表达式里
inline constexpr default_tag_t default_tag{};

namespace detail {

template<typename T>
struct is_default_tag : std::false_type {
};
template<>
struct is_default_tag<default_tag_t> : std::true_type {
};

template<typename T>
inline constexpr bool is_default_tag_v = is_default_tag<T>::value;

// 递归终止：无匹配也无 default
template<typename R, typename T>
constexpr R whenImpl(const T&)
{
    return R();
}

// 主递归：匹配 v == case_val，执行 func
template<typename R, typename T, typename CaseVal, typename Func, typename... Rest>
constexpr R whenImpl(const T& v, CaseVal&& case_val, Func&& func, Rest&&... rest)
{
    if constexpr (is_default_tag_v<std::remove_cvref_t<CaseVal>>) {
        return std::forward<Func>(func)();
    }
    else {
        if (v == std::forward<CaseVal>(case_val)) {
            return std::forward<Func>(func)();
        }
        else {
            return whenImpl<R>(v, std::forward<Rest>(rest)...);
        }
    }
}

template<typename... Args>
struct WhenResult {
    using type = void;
};

template<typename CaseVal1, typename Func1, typename... Rest>
struct WhenResult<CaseVal1, Func1, Rest...> : std::invoke_result<Func1> {
};

template<typename... Args>
using WhenResult_t = typename WhenResult<Args...>::type;

// clang-format off
template<typename R, typename T, typename CaseVal, typename Func, typename... Rest>
struct IsWhenArgs : std::conjunction<
    std::negation< is_default_tag<std::remove_cvref_t<CaseVal>> >,
    std::is_invocable<Func>,
    std::is_same<R, std::invoke_result_t<Func>>,
    std::bool_constant<
        requires(const T ct, const CaseVal cv) {
            { ct == cv } noexcept -> std::convertible_to<bool>;
        }
    >,
    IsWhenArgs<R, T, Rest...>
> {
};
// clang-format on

template<typename R, typename T, typename CaseVal, typename Func>
struct IsWhenArgs<R, T, CaseVal, Func> : std::true_type {
};

template<typename R, typename T, typename... Args>
inline constexpr bool is_when_args_v = IsWhenArgs<R, T, Args...>::value;

template<typename CaseVal, typename Func, typename... Rest>
struct IsNothrowWhenArgs : std::conjunction<std::is_nothrow_invocable<Func>, IsNothrowWhenArgs<Rest...>> {
};

template<typename CaseVal, typename Func>
struct IsNothrowWhenArgs<CaseVal, Func> : std::true_type {
};

template<typename... Args>
inline constexpr bool is_nothrow_when_args_v = IsNothrowWhenArgs<Args...>::value;

}  // namespace detail

/**
 * when 表达式：更加通用的 switch
 *
 * 对于 switch 表达式：
 * @code{cpp}
 * switch (value) {
 * case 1: doSomething1(); break;
 * case 2: doSomething2(); break;
 * default: doDefault(); break;
 * }
 * @endcode
 * 它等效于：
 * @code{cpp}
 * emdevif::when(value,
 *     1, [&] { doSomething1(); },
 *     2, [&] { doSomething2(); },
 *     emdevif::default_tag, [&]{ doDefault(); }
 * );
 * @endcode
 *
 * when 表达式比 switch 更加通用：case 标签后面的值必须是整型的常量表达式，而 when
 * 表达式只要是支持用 `==` 运算符进行比较的，并且不抛出异常，就可以使用。
 *
 * 另外，还仿照 Kotlin 的 when 表达式设计，允许 when 表达式返回值，比如：
 * @code{cpp}
 * int r = emdevif::when(value,
 *     1, [&] { return -1; },
 *     2, [&] { return -2; }  // 注意，这两个 lambda 返回类型必须完全相同（都是隐式推导为 int）
 * );
 * @endcode
 * 如果 value 的值是 2，那么最终 r 等于 -2
 *
 * 另外，在没有显式写出 emdevif::default_tag 的情况下，如果最终没有匹配的值，那么这个 when
 * 表达式将返回默认构造的返回值（例如：如果 value 的值是 -1，那么没有匹配的值，最终 r 就等于 `int{}` 的值）
 *
 * @attention when 表达式的参数应该成对出现（case/default 标签 + lambda），并且如果使用了 default
 * 标签，那么它必须是最后一对参数
 */
template<typename T, typename... Args>
constexpr decltype(auto) when(const T& v, Args&&... args) noexcept(detail::is_nothrow_when_args_v<Args...>)
{
    static_assert(sizeof...(Args) % 2 == 0, "Arguments should be in pairs of (target, func) or (default_tag, func)");

    using ResultType = detail::WhenResult_t<Args...>;
    static_assert(detail::is_when_args_v<ResultType, T, Args...>,
                  "All cases value should be comparable with 'equals'(operator ==) without throw any exceptions, and "
                  "all functions arguments should be invocable with no parameters. Pay attention to that, the "
                  "`default_tag`-`default_func` must be the last argument pair if use default explicitly");

    return detail::whenImpl<ResultType>(v, std::forward<Args>(args)...);
}

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_UTILS_WHEN_HPP
