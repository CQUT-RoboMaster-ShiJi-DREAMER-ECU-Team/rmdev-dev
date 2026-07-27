/**
 * @file message_manager.cppm
 * @brief 消息管理器，用于线程间/线程与中断之间通信
 */

module;

#include "rmdev/message_manager.hpp"

export module rmdev.message_manager;

export import rmdev.message_manager.subscriber;
export import rmdev.message_manager.topic;
