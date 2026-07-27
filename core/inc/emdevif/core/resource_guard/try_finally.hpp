/**
 * @file try_finally.hpp
 * @brief try-finally 模式
 */

#pragma once
#ifndef EMDEVIF_CORE_RESOURCE_GUARD_TRY_FINALLY_HPP
#define EMDEVIF_CORE_RESOURCE_GUARD_TRY_FINALLY_HPP

#include <concepts>
#include <functional>
#include <utility>

#include "emdevif/core/resource_guard/defer.hpp"

namespace emdevif {

/**
 * @brief 有效的 try 块函数概念（可调用且不限制返回值）
 * @tparam Func 函数类型
 */
template<typename Func>
concept ValidTryFunction = std::is_invocable_v<Func>;

/**
 * @brief 有效的 finally 块函数概念（可调用不抛异常，返回 void）
 * @tparam Func 函数类型
 */
template<typename Func>
concept ValidFinallyFunction = std::is_nothrow_invocable_r_v<void, Func>;

/**
 * @brief try-finally 模式的实现
 * @details 执行 try_block，无论是否抛出异常，在返回前都会执行 finally 中的清理逻辑
 * @tparam TryFunc try 块函数类型
 * @tparam FinallyFunc finally 块函数类型
 * @param try_block try 块函数
 * @param finally finally 块函数
 * @return try_block 的返回值
 */
template<ValidTryFunction TryFunc, ValidFinallyFunction FinallyFunc>
constexpr auto tryFinally(TryFunc&& try_block, FinallyFunc&& finally) -> std::invoke_result_t<TryFunc>
{
    [[maybe_unused]] Defer _{[&]() noexcept { std::invoke(std::forward<FinallyFunc>(finally)); }};
    return std::invoke(std::forward<TryFunc>(try_block));
}

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_RESOURCE_GUARD_TRY_FINALLY_HPP
