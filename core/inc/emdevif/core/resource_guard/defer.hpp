/**
 * @file defer.hpp
 * @brief 延迟执行守卫
 */

#pragma once
#ifndef EMDEVIF_CORE_RESOURCE_GUARD_DEFER_HPP
#define EMDEVIF_CORE_RESOURCE_GUARD_DEFER_HPP

#include <concepts>
#include <functional>
#include <utility>

#include "emdevif/core/simplify_decl_macros.hpp"

namespace emdevif {

/**
 * @brief 延迟执行函数概念（要求可调用且不抛异常，返回 void）
 * @tparam Func 函数类型
 */
template<typename Func>
concept DeferFunction = std::is_nothrow_invocable_r_v<void, Func>;

/**
 * @brief 延迟执行守卫（析构时自动调用传入的函数）
 * @tparam Func 延迟执行的函数类型
 */
template<DeferFunction Func>
class Defer final
{
public:
    using FunctionType = Func;

private:
    FunctionType func_;  ///< 存储的延迟执行函数

public:
    /**
     * 构造 Defer 守卫
     * @param func 延迟执行的函数
     */
    explicit constexpr Defer(const FunctionType& func) : func_(func) {}
    /**
     * @overload
     * 以移动方式构造 Defer 守卫
     * @param func 延迟执行的函数
     */
    explicit constexpr Defer(FunctionType&& func) : func_(std::move(func)) {}

    Defer() = delete;
    EMDEVIF_DELETE_COPY_AND_MOVE(Defer);

    constexpr ~Defer() noexcept
    {
        std::invoke(func_);
    }
};

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_RESOURCE_GUARD_DEFER_HPP
