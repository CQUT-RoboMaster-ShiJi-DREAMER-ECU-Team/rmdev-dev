/**
 * @file sys_queue.inl
 * @brief
 */

#pragma once
#ifndef EMDEVIF_FREERTOS_SYSTEM_IMPL_SYS_QUEUE_INL
    #define EMDEVIF_FREERTOS_SYSTEM_IMPL_SYS_QUEUE_INL

    #include "emdevif/core/detail/config.hpp"

    #if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
        #include "freertos/FreeRTOS.h"
        #include "freertos/queue.h"
    #else
        #include "FreeRTOS.h"
        #include "queue.h"
    #endif

    #include "emdevif/core/fatal_handler.h"

    #include "emdevif/core/error_handler.hpp"

    #include <cstddef>

namespace emdevif {

template<typename Type, std::size_t item_size>
class SysQueueStaticInstance
{
public:
    SysQueueStaticInstance() noexcept : instance(), queue_buffer() {}

    explicit operator StaticQueue_t&() noexcept
    {
        return instance;
    }

    explicit operator void*() noexcept
    {
        return queue_buffer;
    }

private:
    StaticQueue_t instance;        ///< 静态队列实例
    Type queue_buffer[item_size];  ///< 队列数据存储缓冲区
};

template<typename Type, std::size_t item_size>
SysQueue<Type, item_size>::SysQueue(SysQueueBuilder builder) noexcept : handle_(nullptr)
{
    if (builder.static_instance != nullptr) {
        auto& static_instance = *static_cast<SysQueueStaticInstance<Type, item_size>*>(builder.static_instance);

        handle_ = xQueueCreateStatic(item_size,
                                     sizeof(Type),
                                     static_cast<uint8_t*>(static_cast<void*>(static_instance)),
                                     &static_cast<StaticQueue_t&>(static_instance));
    }
    else {
        handle_ = xQueueCreate(item_size, sizeof(Type));
    }

    #if (configQUEUE_REGISTRY_SIZE > 0)
    if (handle_ != nullptr) {
        vQueueAddToRegistry(static_cast<QueueHandle_t>(handle_), builder.name);
    }
    #endif
}

template<typename Type, std::size_t item_size>
void SysQueue<Type, item_size>::destroy(SysQueue& obj) noexcept
{
    if (obj.handle_ != nullptr) {
        vQueueDelete(static_cast<QueueHandle_t>(obj.handle_));

    #if (configQUEUE_REGISTRY_SIZE > 0)
        vQueueUnregisterQueue(static_cast<QueueHandle_t>(obj.handle_));
    #endif

        obj.handle_ = nullptr;
    }
}

template<typename Type, std::size_t item_size>
ErrorCode SysQueue<Type, item_size>::push(const bool in_isr,
                                          const Type& data,
                                          const MessageQueueTimeout_t timeout_tick) noexcept
{
    if (in_isr) {
        EMDEVIF_ASSERT(timeout_tick == 0U, "Timeout value should equals to 0 in ISR.");

        BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
        const auto ret = xQueueSendFromISR(static_cast<QueueHandle_t>(handle_), &data, &xHigherPriorityTaskWokenByPost);

        ErrorCode final_ret;

        if (ret == pdTRUE) {
            final_ret = ErrorCode::Success;
        }
        else {
            final_ret = ErrorCode::Full;
        }

        if (xHigherPriorityTaskWokenByPost) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWokenByPost);
        }

        return final_ret;
    }
    else {
        const auto ret = xQueueSend(static_cast<QueueHandle_t>(handle_), &data, timeout_tick);
        if (ret == pdTRUE) {
            return ErrorCode::Success;
        }
        else {
            return ErrorCode::Full;
        }
    }
}

template<typename Type, std::size_t item_size>
ErrorCode SysQueue<Type, item_size>::forcePush(const bool in_isr, const Type& data) noexcept
{
    if (in_isr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        auto ret = xQueueOverwriteFromISR(static_cast<QueueHandle_t>(handle_), &data, &xHigherPriorityTaskWoken);
        EMDEVIF_ASSERT(ret == pdPASS, "Failed to overwrite queue data in ISR context");

        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        return ErrorCode::Success;
    }
    else {
        auto ret = xQueueOverwrite(static_cast<QueueHandle_t>(handle_), &data);
        EMDEVIF_ASSERT(ret == pdPASS, "Failed to overwrite queue data in non-ISR context");

        return ErrorCode::Success;
    }
}

template<typename Type, std::size_t item_size>
ErrorCode SysQueue<Type, item_size>::pop(const bool in_isr,
                                         Type& data,
                                         const MessageQueueTimeout_t timeout_tick) noexcept
{
    if (in_isr) {
        EMDEVIF_ASSERT(timeout_tick == 0U, "Timeout value should equals to 0 in ISR.");

        BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
        const auto ret =
            xQueueReceiveFromISR(static_cast<QueueHandle_t>(handle_), &data, &xHigherPriorityTaskWokenByPost);

        ErrorCode final_ret;

        if (ret == pdTRUE) {
            final_ret = ErrorCode::Success;
        }
        else {
            final_ret = ErrorCode::Empty;
        }

        if (xHigherPriorityTaskWokenByPost) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWokenByPost);
        }

        return final_ret;
    }
    else {
        const auto ret = xQueueReceive(static_cast<QueueHandle_t>(handle_), &data, timeout_tick);
        if (ret == pdTRUE) {
            return ErrorCode::Success;
        }
        else {
            return ErrorCode::Empty;
        }
    }
}

template<typename Type, std::size_t item_size>
ErrorCode SysQueue<Type, item_size>::pop(const bool in_isr) noexcept
{
    Type data;
    return pop(in_isr, data, 0U);
}

template<typename Type, std::size_t item_size>
ErrorCode SysQueue<Type, item_size>::peek(const bool in_isr,
                                          Type& data,
                                          const MessageQueueTimeout_t timeout_tick) noexcept
{
    if (in_isr) {
        EMDEVIF_ASSERT(timeout_tick == 0U, "Timeout value should equals to 0 in ISR.");

        const auto ret = xQueuePeekFromISR(static_cast<QueueHandle_t>(handle_), &data);

        ErrorCode final_ret;

        if (ret == pdTRUE) {
            final_ret = ErrorCode::Success;
        }
        else {
            final_ret = ErrorCode::Empty;
        }

        return final_ret;
    }
    else {
        const auto ret = xQueuePeek(static_cast<QueueHandle_t>(handle_), &data, timeout_tick);
        if (ret == pdTRUE) {
            return ErrorCode::Success;
        }
        else {
            return ErrorCode::Empty;
        }
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
template<typename Type, std::size_t item_size>
void SysQueue<Type, item_size>::clear() noexcept
{
    (void)xQueueReset(static_cast<QueueHandle_t>(handle_));
}

template<typename Type, std::size_t item_size>
std::size_t SysQueue<Type, item_size>::storeCount() const noexcept
{
    return uxQueueMessagesWaiting(static_cast<QueueHandle_t>(handle_));
}

template<typename Type, std::size_t item_size>
std::size_t SysQueue<Type, item_size>::remainCount() const noexcept
{
    return uxQueueSpacesAvailable(static_cast<QueueHandle_t>(handle_));
}

template<typename Type, std::size_t item_size>
SysQueue<Type, item_size>::~SysQueue()
{
    if (handle_ != nullptr) {
        this->destroy();
    }
}

}  // namespace emdevif

#endif  // !EMDEVIF_FREERTOS_SYSTEM_IMPL_SYS_QUEUE_INL
