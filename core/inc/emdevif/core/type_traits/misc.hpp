/**
 * @file misc.hpp
 * @brief 杂项
 */

#pragma once
#ifndef EMDEVIF_CORE_TYPE_TRAITS_MISC_HPP
#define EMDEVIF_CORE_TYPE_TRAITS_MISC_HPP

#include <type_traits>

namespace emdevif {

/**
 * 判断表达式是否处于编译期求值上下文
 *
 * 使用示例：
 * @code
 * constexpr auto a = 1;
 * static_assert(emdevif::is_consteval([] { return a; }));  // 需要传入一个 lambda 表达式
 *
 * auto b = 0;
 * static_assert(!emdevif::is_consteval([] { return b; }));
 * @endcode
 * @tparam Expr 表达式类型
 * @return 如果表达式是编译期求值上下文，返回 true；否则返回 false
 */
template<typename Expr>
consteval bool is_consteval(Expr)
{
    // 实现参考 https://github.com/microsoft/proxy/issues/105
    return requires { typename std::bool_constant<(Expr{}(), false)>; };
}

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_TYPE_TRAITS_MISC_HPP
