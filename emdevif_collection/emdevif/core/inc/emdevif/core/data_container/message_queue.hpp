/**
 * @file message_queue.hpp
 * @brief 消息队列与消息槽概念
 */

#pragma once
#ifndef EMDEVIF_CORE_DATA_CONTAINER_MESSAGE_QUEUE_HPP
#define EMDEVIF_CORE_DATA_CONTAINER_MESSAGE_QUEUE_HPP

#include <cstddef>
#include <cstdint>

#include <concepts>
#include <type_traits>

#include "emdevif/core/error_handler.hpp"

namespace emdevif {

#ifndef EMDEVIF_CORE_DATA_CONTAINER_MESSAGE_QUEUE_TIMEOUT_TYPE_BITS
#define EMDEVIF_CORE_DATA_CONTAINER_MESSAGE_QUEUE_TIMEOUT_TYPE_BITS 64
#endif

#if (EMDEVIF_CORE_DATA_CONTAINER_MESSAGE_QUEUE_TIMEOUT_TYPE_BITS == 16)
using MessageQueueTimeout_t = uint16_t;
#elif (EMDEVIF_CORE_DATA_CONTAINER_MESSAGE_QUEUE_TIMEOUT_TYPE_BITS == 32)
using MessageQueueTimeout_t = uint32_t;
#elif (EMDEVIF_CORE_DATA_CONTAINER_MESSAGE_QUEUE_TIMEOUT_TYPE_BITS == 64)
using MessageQueueTimeout_t = uint64_t;
#else
#error \
    "The value of macro `EMDEVIF_CORE_DATA_CONTAINER_MESSAGE_QUEUE_TIMEOUT_TYPE_BITS' can only equals to 16, 32 or 64."
#endif

/**
 * 消息槽概念
 *
 * 满足该概念的类型提供 forcePush / peek 等接口，容量固定为单一槽位语义。
 * @tparam T 待检查的类型
 */
template<typename T>
concept MessageSlot =
    requires {
        typename T::ValueType;
        T::item_size;
    } && std::same_as<std::remove_cvref_t<decltype(T::item_size)>, std::size_t> && std::is_move_constructible_v<T> &&
    std::is_move_assignable_v<T> && !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T> &&
    requires(T& t,
             const T& ct,
             T u,
             bool in_isr,
             typename T::ValueType& data,
             const typename T::ValueType& cdata,
             MessageQueueTimeout_t timeout) {
        { u = T{T::create({.name = ""})} } noexcept -> std::same_as<T&>;
        { T::destroy(u) } noexcept -> std::same_as<void>;
        { T::destroy(t) } noexcept -> std::same_as<void>;
        { u.destroy() } noexcept -> std::same_as<void>;

        /**
         * 强制推送数据到队列（忽略队列满的情况）
         * @param in_isr 是否在中断上下文中调用
         * @param data 待推送的数据
         * @return 错误码
         */
        { t.forcePush(in_isr, cdata) } noexcept -> std::same_as<ErrorCode>;

        /**
         * 查看队列头部数据（不移除）
         * @param in_isr 是否在中断上下文中调用
         * @param[out] data 接收查看的数据
         * @param timeout_tick 超时时间（tick 数），默认 0 表示不等待
         * @return 错误码
         */
        { t.peek(in_isr, data, timeout) } noexcept -> std::same_as<ErrorCode>;

        /**
         * 清空队列
         */
        { t.clear() } noexcept -> std::same_as<void>;

        /**
         * 获取队列中已存储的元素数量
         * @return 已存储的元素数量
         */
        { ct.storeCount() } noexcept -> std::same_as<std::size_t>;

        /**
         * 获取队列中剩余的空闲槽位数量
         * @return 剩余空闲槽位数量
         */
        { ct.remainCount() } noexcept -> std::same_as<std::size_t>;

        /**
         * 获取队列最大容量
         * @return 队列最大容量
         */
        { T::maxItemCount() } noexcept -> std::same_as<std::size_t>;

        /**
         * 获取队列底层句柄（用于 RTOS API 操作）
         * @return 队列句柄
         */
        { ct.getHandle() } noexcept -> std::same_as<void*>;
    };

/**
 * 消息队列概念
 *
 * 满足 MessageSlot 且额外提供 push / pop 接口的类型。
 * @tparam T 待检查的类型
 */
template<typename T>
concept MessageQueue = MessageSlot<T> && requires(T& t,
                                                  bool in_isr,
                                                  typename T::ValueType& data,
                                                  const typename T::ValueType& cdata,
                                                  MessageQueueTimeout_t timeout) {
    /**
     * 推送数据到队列
     * @param in_isr 是否在中断上下文中调用
     * @param cdata 待推送的数据
     * @param timeout_tick 超时时间（tick 数），默认 0 表示不等待
     * @return 错误码
     */
    { t.push(in_isr, cdata, timeout) } noexcept -> std::same_as<ErrorCode>;

    /**
     * 从队列弹出数据
     * @param in_isr 是否在中断上下文中调用
     * @param[out] data 接收弹出的数据
     * @param timeout_tick 超时时间（tick 数），默认 0 表示不等待
     * @return 错误码
     */
    { t.pop(in_isr, data, timeout) } noexcept -> std::same_as<ErrorCode>;

    /**
     * 从队列弹出数据（不获取数据内容，只移除元素）
     * @param in_isr 是否在中断上下文中调用
     * @return 错误码
     */
    { t.pop(in_isr) } noexcept -> std::same_as<ErrorCode>;
};

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_DATA_CONTAINER_MESSAGE_QUEUE_HPP
