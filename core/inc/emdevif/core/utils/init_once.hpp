/**
 * @file init_once.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_CORE_UTILS_INIT_ONCE_HPP
#define EMDEVIF_CORE_UTILS_INIT_ONCE_HPP

#include <type_traits>
#include <utility>

#include "emdevif/core/error_handler.hpp"

namespace emdevif {

/**
 * 单次初始化类
 * @tparam Type 存储的数据类型
 */
template<typename Type>
    requires(std::is_copy_constructible_v<Type> && std::is_default_constructible_v<Type> &&
             std::is_move_constructible_v<Type>)
class InitOnce
{
public:
    /**
     * 默认构造函数（将会调用封装的默认构造函数，后续可以再赋值一次）
     */
    constexpr InitOnce() : v(), is_init(false) {}

    ~InitOnce() = default;

    /**
     * 拷贝构造函数
     * @param other 另一个实例
     */
    constexpr InitOnce(const InitOnce& other) : v(other.v), is_init(true) {}

    /**
     * 移动构造函数（将会移动数据的资源）
     * @param other 另一个实例
     */
    constexpr InitOnce(InitOnce&& other) noexcept : v(std::move(other.v)), is_init(true) {}

    /**
     * 初始化值
     * @param other_value 待初始化的值
     * @return 错误码。若已被初始化后，返回 ErrorCode::AlreadyExists。
     */
    constexpr ErrorCode init(const Type& other_value)
    {
        if (is_init) {
            return ErrorCode::AlreadyExists;
        }

        v = other_value;
        is_init = true;

        return ErrorCode::Success;
    }

    /**
     * 初始化值 - 移动语义
     * @param other_value 待初始化的值
     * @return 错误码。若已被初始化后，返回 ErrorCode::AlreadyExists。
     */
    constexpr ErrorCode init(Type&& other_value)
    {
        if (is_init) {
            return ErrorCode::AlreadyExists;
        }

        v = std::move(other_value);
        is_init = true;

        return ErrorCode::Success;
    }

    /**
     * 从另一个实例中拷贝数据
     * @param other 另一个实例
     * @return this
     */
    constexpr InitOnce& operator=(const InitOnce& other)
    {
        init(other.v);

        return *this;
    }

    /**
     * 从另一个实例中移动数据
     * @param other 另一个实例
     * @return this
     */
    constexpr InitOnce& operator=(InitOnce&& other) noexcept
    {
        init(std::move(other.v));

        return *this;
    }

    /**
     * 从另一个值中拷贝数据并构造
     * @param other_value 另一个值
     */
    explicit constexpr InitOnce(const Type& other_value) : v(other_value), is_init(true) {}

    /**
     * 从另一个值中移动数据并构造
     * @param other_value 另一个值
     */
    explicit constexpr InitOnce(Type&& other_value) noexcept : v(std::move(other_value)), is_init(true) {}

    /**
     * 从另一个值拷贝/移动数据
     * @tparam Type_ 数据的类型
     * @param other_value 另一个值
     * @return this
     */
    template<typename Type_>
    constexpr InitOnce& operator=(Type_&& other_value)
    {
        init(std::forward<Type_>(other_value));

        return *this;
    }

    /**
     * 获得值的常量引用
     * @return 值的常量引用
     */
    constexpr const Type& operator()() const
    {
        return v;
    }

    /**
     * @brief 隐式转换为存储的类型
     * @return 存储的值
     */
    constexpr operator Type() const  // NOLINT(google-explicit-constructor): 允许隐式转换成为自身类型
    {
        return v;
    }

private:
    Type v;        ///< 值

    bool is_init;  ///< 是否已经初始化
};

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_UTILS_INIT_ONCE_HPP
