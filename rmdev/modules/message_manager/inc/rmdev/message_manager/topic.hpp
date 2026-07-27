/**
 * @file topic.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_MESSAGE_MANAGER_TOPIC_HPP
#define RMDEV_MESSAGE_MANAGER_TOPIC_HPP

#include <cstddef>
#include <cstdint>

#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/data_container/message_queue.hpp"
#include "emdevif/core/error_handler.hpp"
#include "rmdev/message_manager/subscriber.hpp"

namespace rmdev {

/**
 * @brief 消息主题，管理一组消息队列/槽，提供发布和订阅功能
 * @tparam QueueImpl 底层队列/槽实现类型
 * @tparam Allocator 分配器类型，默认为 std::allocator
 */
template<emdevif::MessageSlot QueueImpl, template<typename AllocatorType> typename Allocator = std::allocator>
class Topic
{
public:
    using ValueType = QueueImpl::ValueType;

    static constexpr std::size_t queue_item_size = QueueImpl::item_size;

    static constexpr bool is_queue_not_slot = emdevif::MessageQueue<QueueImpl>;

    using QueueListType = std::vector<QueueImpl, Allocator<QueueImpl>>;
    using QueueListIterator = QueueListType::iterator;
    using QueueListConstIterator = QueueListType::const_iterator;

private:
    std::string_view name_;

    QueueListType queue_list_;

public:
    explicit Topic(const std::string_view name, std::size_t initial_queue_list_buffer_count = 3) : name_(name)
    {
        queue_list_.reserve(initial_queue_list_buffer_count);
    }

    /**
     * @brief 获取订阅者队列列表的引用
     * @return 队列列表引用
     */
    QueueListType& getQueueListInstance()
    {
        return queue_list_;
    }
    /**
     * @brief 获取订阅者队列列表的常量引用
     * @return 队列列表常量引用
     */
    const QueueListType& getQueueListInstance() const
    {
        return queue_list_;
    }

    /**
     * @brief 获取主题名称
     * @return 主题名称
     */
    std::string_view getName() const
    {
        return name_;
    }

    /**
     * @brief 添加一个新的订阅者
     * @tparam SubValueType 订阅者视角的数据类型，默认为 ValueType
     * @return 新创建的订阅者实例
     */
    template<class SubValueType = ValueType>
    Subscriber<SubValueType, QueueImpl> addSubscriber()
    {
        queue_list_.emplace_back(QueueImpl::create({.name = ""}));
        return Subscriber<SubValueType, QueueImpl>{queue_list_.back()};
    }

    /**
     * @brief 向所有订阅者推送消息（仅消息队列支持，消息槽不支持）
     * @param in_isr 是否在中断上下文中调用
     * @param data 消息数据
     * @param timeout_tick 超时时间（tick 数），默认 0 表示不等待
     * @return 错误码与失败的队列迭代器的键值对
     */
    auto push(bool in_isr, const ValueType& data, emdevif::MessageQueueTimeout_t timeout_tick = 0U)
        -> std::pair<emdevif::ErrorCode, QueueListIterator>
    {
        using emdevif::ErrorCode;

        if constexpr (is_queue_not_slot) {
            for (auto it = queue_list_.begin(); it != queue_list_.end(); ++it) {
                if (ErrorCode ret = it->push(in_isr, data, timeout_tick); ret != ErrorCode::Success) {
                    return {ret, it};
                }
            }

            return {ErrorCode::Success, queue_list_.end()};
        }
        else {
            EMDEVIF_FATAL_HANDLER(
                "It's message slot, method 'push' was not supported. Please use 'forcePush' instead.");
            return {ErrorCode::NotImplemented, queue_list_.end()};
        }
    }

    /**
     * @brief 向所有订阅者强制推送消息（覆盖旧数据）
     * @param in_isr 是否在中断上下文中调用
     * @param data 消息数据
     * @return 错误码与失败的队列迭代器的键值对
     */
    auto forcePush(bool in_isr, const ValueType& data) -> std::pair<emdevif::ErrorCode, QueueListIterator>
    {
        using emdevif::ErrorCode;

        for (auto it = queue_list_.begin(); it != queue_list_.end(); ++it) {
            if (ErrorCode ret = it->forcePush(in_isr, data); ret != ErrorCode::Success) {
                return {ret, it};
            }
        }

        return {ErrorCode::Success, queue_list_.end()};
    }
};

}  // namespace rmdev

#endif  // !RMDEV_MESSAGE_MANAGER_TOPIC_HPP
