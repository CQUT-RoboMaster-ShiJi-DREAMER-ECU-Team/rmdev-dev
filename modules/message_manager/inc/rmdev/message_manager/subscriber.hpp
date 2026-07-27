/**
 * @file subscriber.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_MESSAGE_MANAGER_SUBSCRIBER_HPP
#define RMDEV_MESSAGE_MANAGER_SUBSCRIBER_HPP

#include <cstddef>

#include <type_traits>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/data_container/message_queue.hpp"
#include "emdevif/core/error_handler.hpp"

namespace rmdev {

/**
 * @brief 消息订阅者，提供 pop/peek 等接口从消息队列或消息槽中读取数据
 * @tparam ViewType_ 用户视角的数据类型
 * @tparam QueueImpl 底层队列/槽实现类型
 */
template<typename ViewType_, emdevif::MessageSlot QueueImpl>
class Subscriber
{
public:
    using ViewType = ViewType_;

    static constexpr bool is_queue_not_slot = emdevif::MessageQueue<QueueImpl>;

    using ValueType = QueueImpl::ValueType;

    static_assert(std::is_convertible_v<ValueType, ViewType>, "`ValueType\' should be convertable to `ViewType\'");

    static constexpr bool view_type_equals_to_value_type = std::is_same_v<ValueType, ViewType>;

private:
    QueueImpl& queue_;

public:
    explicit Subscriber(QueueImpl& queue) : queue_(queue) {}

    /**
     * @brief 从队列中弹出数据（仅消息队列支持，消息槽不支持）
     * @param in_isr 是否在中断上下文中调用
     * @param[out] data 接收消息数据的引用
     * @param timeout_tick 超时时间（tick 数），默认 0 表示不等待
     * @return 错误码
     */
    emdevif::ErrorCode pop(bool in_isr, ViewType& data, emdevif::MessageQueueTimeout_t timeout_tick = 0U)
    {
        if constexpr (is_queue_not_slot) {
            if constexpr (!view_type_equals_to_value_type) {
                ValueType origin_data;

                auto ret = queue_.pop(in_isr, origin_data, timeout_tick);
                data = static_cast<ViewType>(origin_data);

                return ret;
            }
            else {
                return queue_.pop(in_isr, data, timeout_tick);
            }
        }
        else {
            EMDEVIF_FATAL_HANDLER("It's message slot, method 'pop' was not supported. Please use 'peek' instead.");
        }
    }
    /**
     * @overload
     * @brief 从队列中弹出并丢弃一条消息（仅消息队列支持）
     * @param in_isr 是否在中断上下文中调用
     * @return 错误码
     */
    emdevif::ErrorCode pop(bool in_isr)
    {
        if constexpr (is_queue_not_slot) {
            return queue_.pop(in_isr);
        }
        else {
            EMDEVIF_FATAL_HANDLER("It's message slot, method 'pop' was not supported. Please use 'peek' instead.");
        }
    }

    /**
     * @brief 从队列/槽中读取数据但不移除（peek）
     * @param in_isr 是否在中断上下文中调用
     * @param[out] data 接收消息数据的引用
     * @param timeout_tick 超时时间（tick 数），默认 0 表示不等待
     * @return 错误码
     */
    emdevif::ErrorCode peek(bool in_isr, ViewType& data, emdevif::MessageQueueTimeout_t timeout_tick = 0U)
    {
        if constexpr (!view_type_equals_to_value_type) {
            ValueType origin_data;

            auto ret = queue_.peek(in_isr, origin_data, timeout_tick);
            data = static_cast<ViewType>(origin_data);

            return ret;
        }
        else {
            return queue_.peek(in_isr, data, timeout_tick);
        }
    }

    /**
     * @brief 清空队列/槽中的所有消息
     */
    void clear()
    {
        queue_.clear();
    }

    /**
     * @brief 获取队列/槽中已存储的消息数量
     * @return 消息数量
     */
    [[nodiscard]] std::size_t storeCount() const
    {
        return queue_.storeCount();
    }

    /**
     * @brief 获取队列/槽的剩余可用容量
     * @return 剩余容量
     */
    [[nodiscard]] std::size_t remainCount() const
    {
        return queue_.remainCount();
    }

    /**
     * @brief 获取队列/槽的最大容量
     * @return 最大容量
     */
    [[nodiscard]] static constexpr std::size_t maxItemCount()
    {
        return QueueImpl::item_size;
    }

    /**
     * @brief 获取底层队列/槽的操作句柄
     * @return 句柄指针
     */
    [[nodiscard]] void* getHandle() const
    {
        return queue_.getHandle();
    }

    /**
     * @brief 获取底层队列/槽实例的引用
     * @return 队列/槽实例引用
     */
    [[nodiscard]] QueueImpl& getQueueInstance()
    {
        return queue_;
    }
    /**
     * @brief 获取底层队列/槽实例的常量引用
     * @return 队列/槽实例常量引用
     */
    [[nodiscard]] const QueueImpl& getQueueInstance() const
    {
        return queue_;
    }
};

}  // namespace rmdev

#endif  // !RMDEV_MESSAGE_MANAGER_SUBSCRIBER_HPP
