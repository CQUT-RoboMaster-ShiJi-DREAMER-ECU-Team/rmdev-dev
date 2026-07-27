/**
 * @file message_queue.cppm
 * @brief 消息队列接口
 */

module;

#include "emdevif/core/data_container/message_queue.hpp"

export module emdevif.core.data_container.message_queue;

export namespace emdevif {
using ::emdevif::MessageQueue;
using ::emdevif::MessageQueueTimeout_t;
using ::emdevif::MessageSlot;
}  // namespace emdevif
