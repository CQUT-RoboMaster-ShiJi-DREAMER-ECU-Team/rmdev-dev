/**
 * @file sys_queue.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_SYSTEM_SYS_QUEUE_HPP
#define EMDEVIF_SYSTEM_SYS_QUEUE_HPP

#include <cstddef>
#include <cstdint>

#include <concepts>
#include <type_traits>
#include <utility>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/data_container/message_queue.hpp"
#include "emdevif/core/error_handler.hpp"
#include "emdevif/system/thread.hpp"

namespace emdevif {

/**
 * 系统队列的静态实例
 * @copydoc sys_static_instance
 * @tparam Type 队列元素类型
 * @tparam item_size 队列大小
 */
template<typename Type, std::size_t item_size>
class SysQueueStaticInstance;

/**
 * 系统队列 Builder
 */
struct SysQueueBuilder {
    const char* name{};              ///< 名称

    void* static_instance{nullptr};  ///< 静态实例内存
};

/**
 * 系统队列
 * @tparam Type 队列每个元素的类型
 * @tparam item_size_ 队列的大小
 */
template<typename Type, std::size_t item_size_>
class SysQueue
{
public:
    using ValueType = Type;  ///< 消息元素类型
    using Handle = void*;    ///< 句柄

    static constexpr std::size_t item_size = item_size_;

    // todo 等消息队列接口做好了之后再来补注释
    /**
     * 通过 Builder 构造系统队列
     * @param builder Builder
     */
    explicit SysQueue(SysQueueBuilder builder) noexcept;

    /**
     * @brief 将 Builder 原样返回，用于泛型代码中通过 QueueImpl::create({...}) 统一创建
     * @param builder Builder
     * @return 传入的 Builder
     */
    static SysQueueBuilder create(SysQueueBuilder builder) noexcept
    {
        return builder;
    }

    static void destroy(SysQueue& obj) noexcept;

    void destroy() noexcept
    {
        destroy(*this);
        handle_ = nullptr;
    }

    /**
     * @brief 向队列尾部推送数据
     * @param in_isr 是否在中断上下文中调用
     * @param data 待推送的数据
     * @param timeout_tick 超时 tick 数，0 表示不等待
     * @retval ErrorCode::Success 推送成功
     * @retval ErrorCode::Timeout 超时
     */
    ErrorCode push(bool in_isr, const Type& data, MessageQueueTimeout_t timeout_tick = 0U) noexcept;

    /**
     * @brief 强制向队列尾部推送数据（覆盖旧数据）
     * @param in_isr 是否在中断上下文中调用
     * @param data 待推送的数据
     * @retval ErrorCode::Success 推送成功
     * @retval ErrorCode::OperationFail 操作失败
     */
    ErrorCode forcePush(bool in_isr, const Type& data) noexcept;

    /**
     * @brief 从队列头部取出数据
     * @param[out] data 用于接收取出的数据
     * @param in_isr 是否在中断上下文中调用
     * @param timeout_tick 超时 tick 数，0 表示不等待
     * @retval ErrorCode::Success 取出成功
     * @retval ErrorCode::Timeout 超时
     */
    ErrorCode pop(bool in_isr, Type& data, MessageQueueTimeout_t timeout_tick = 0U) noexcept;

    /**
     * @brief 从队列头部取出数据（丢弃数据）
     * @param in_isr 是否在中断上下文中调用
     * @retval ErrorCode::Success 取出成功
     * @retval ErrorCode::Timeout 超时
     */
    ErrorCode pop(bool in_isr) noexcept;

    /**
     * @brief 查看队列头部的数据但不移除
     * @param[out] data 用于接收头部数据
     * @param in_isr 是否在中断上下文中调用
     * @param timeout_tick 超时 tick 数，0 表示不等待
     * @retval ErrorCode::Success 查看成功
     * @retval ErrorCode::Timeout 超时
     */
    ErrorCode peek(bool in_isr, Type& data, MessageQueueTimeout_t timeout_tick = 0U) noexcept;

    /**
     * @brief 清空队列中的所有数据
     */
    void clear() noexcept;

    /**
     * @brief 获取队列中已存储的元素数量
     * @return 已存储的元素数量
     */
    [[nodiscard]] std::size_t storeCount() const noexcept;

    /**
     * @brief 获取队列中剩余可用空间
     * @return 剩余可存储的元素数量
     */
    [[nodiscard]] std::size_t remainCount() const noexcept;

    [[nodiscard]] Handle getHandle() const noexcept
    {
        return handle_;
    }

    [[nodiscard]] static constexpr std::size_t maxItemCount() noexcept
    {
        return item_size_;
    }

    SysQueue() noexcept : handle_(nullptr) {}

    SysQueue& operator=(const SysQueue&) = delete;
    SysQueue(const SysQueue&) = delete;

    SysQueue(SysQueue&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}

    SysQueue& operator=(SysQueue&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        if (handle_ != nullptr) {
            EMDEVIF_FATAL_HANDLER("Should not move-assign to a non-empty message queue!");
            return *this;
        }

        this->handle_ = std::exchange(other.handle_, nullptr);

        return *this;
    }

    ~SysQueue();

private:
    Handle handle_;  ///< 底层实现的句柄
};

static_assert(MessageQueue<SysQueue<int, 5>>);

static_assert(std::same_as<emdevif::SysTick_t, MessageQueueTimeout_t>,
              "emdevif::SysTick_t and MessageQueueTimeout_t must be the same type.");

}  // namespace emdevif

#include "emdevif/system_impl/sys_queue.inl"

#endif  // !EMDEVIF_SYSTEM_SYS_QUEUE_HPP
