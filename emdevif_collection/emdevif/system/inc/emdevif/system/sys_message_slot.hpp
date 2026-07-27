/**
 * @file sys_message_slot.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_SYSTEM_SYS_MESSAGE_SLOT_HPP
#define EMDEVIF_SYSTEM_SYS_MESSAGE_SLOT_HPP

#include <type_traits>
#include <utility>

#include "emdevif/core/data_container/message_queue.hpp"
#include "emdevif/core/error_handler.hpp"
#include "emdevif/system/sys_queue.hpp"
#include "emdevif/system/thread.hpp"

namespace emdevif {

/**
 * 系统消息槽 - 基于系统队列实现，仅暴露 forcePush / peek 接口
 * @tparam Type 消息数据类型
 */
template<typename Type>
class SysMessageSlot
{
public:
    using ValueType = Type;  ///< 消息元素类型
    using Handle = void*;    ///< 句柄类型

    static constexpr std::size_t item_size = 1;

private:
    SysQueue<Type, 1> queue_{};

public:
    /**
     * 通过 SysQueueBuilder 创建 SysMessageSlot
     * @param builder Builder
     * @return 创建好的 SysMessageSlot
     */
    static SysMessageSlot create(SysQueueBuilder builder) noexcept
    {
        SysMessageSlot slot;
        slot.queue_ = SysQueue<Type, 1>{std::move(builder)};
        return slot;
    }

    static void destroy(SysMessageSlot& obj) noexcept
    {
        obj.queue_.destroy();
    }

    void destroy() noexcept
    {
        destroy(*this);
    }

    /**
     * @brief 强制推送数据到消息槽（覆盖旧数据）
     * @param in_isr 是否在中断上下文中调用
     * @param data 待推送的数据
     * @return 错误码
     */
    ErrorCode forcePush(bool in_isr, const Type& data) noexcept
    {
        return queue_.forcePush(in_isr, data);
    }

    /**
     * @brief 查看消息槽中的数据（不移除）
     * @param in_isr 是否在中断上下文中调用
     * @param[out] data 接收查看的数据
     * @param timeout_tick 超时时间（tick 数），默认 0 表示不等待
     * @return 错误码
     */
    ErrorCode peek(bool in_isr, Type& data, MessageQueueTimeout_t timeout_tick = 0U) noexcept
    {
        return queue_.peek(in_isr, data, timeout_tick);
    }

    /**
     * @brief 清空消息槽
     */
    void clear() noexcept
    {
        queue_.clear();
    }

    /**
     * @brief 获取消息槽中已存储的元素数量
     * @return 已存储的元素数量
     */
    [[nodiscard]] std::size_t storeCount() const noexcept
    {
        return queue_.storeCount();
    }

    /**
     * @brief 获取消息槽中剩余的空闲槽位数量
     * @return 剩余空闲槽位数量
     */
    [[nodiscard]] std::size_t remainCount() const noexcept
    {
        return queue_.remainCount();
    }

    /**
     * @brief 获取消息槽最大容量（固定为 1）
     * @return 消息槽最大容量
     */
    [[nodiscard]] static constexpr std::size_t maxItemCount() noexcept
    {
        return 1;
    }

    /**
     * @brief 获取消息槽底层句柄
     * @return 消息槽句柄
     */
    [[nodiscard]] Handle getHandle() const noexcept
    {
        return queue_.getHandle();
    }

    SysMessageSlot() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template<typename T>
    SysMessageSlot(T&& init_param) noexcept  // NOLINT(*-explicit-constructor, *-forwarding-reference-overload)
        : queue_(std::forward<T>(init_param))
    {
    }

    template<typename T>
    SysMessageSlot& operator=(T&& other) noexcept
    {
        queue_.operator=(std::forward<T>(other));

        return *this;
    }
};

static_assert(MessageSlot<SysMessageSlot<int>>);

}  // namespace emdevif

#endif  // !EMDEVIF_SYSTEM_SYS_MESSAGE_SLOT_HPP