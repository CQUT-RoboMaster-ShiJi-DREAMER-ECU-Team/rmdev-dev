/**
 * @file semaphore.inl
 * @brief
 */

#pragma once
#ifndef EMDEVIF_FREERTOS_SYSTEM_IMPL_SEMAPHORE_INL
    #define EMDEVIF_FREERTOS_SYSTEM_IMPL_SEMAPHORE_INL

    #include "emdevif/core/detail/config.hpp"

    #if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
        #include "freertos/FreeRTOS.h"
        #include "freertos/semphr.h"
    #else
        #include "FreeRTOS.h"
        #include "semphr.h"
    #endif

    #include "emdevif/core/error_handler.hpp"

    #include "emdevif/core/fatal_handler.h"

    #include <cstddef>
    #include <utility>

// for template<std::ptrdiff_t least_max_value> CountingSemaphore
namespace emdevif {

template<std::ptrdiff_t least_max_value>
class CountingSemaphoreStaticInstance
{
public:
    CountingSemaphoreStaticInstance() noexcept : instance() {}

    friend class CountingSemaphore<least_max_value>;

private:
    explicit operator StaticSemaphore_t&() noexcept
    {
        return instance;
    }

private:
    StaticSemaphore_t instance;  ///< 静态创建所需的内存块
};

template<std::ptrdiff_t least_max_value>
CountingSemaphore<least_max_value>::CountingSemaphore(CountingSemaphoreBuilder builder) noexcept : handle_(nullptr)
{
    if (builder.static_instance != nullptr) {
        auto& static_instance =
            *static_cast<CountingSemaphoreStaticInstance<least_max_value>*>(builder.static_instance);

        handle_ =
            xSemaphoreCreateCountingStatic(least_max_value, 0U, &static_cast<StaticSemaphore_t&>(static_instance));
    }
    else {
        handle_ = xSemaphoreCreateCounting(least_max_value, 0U);
    }
}

template<std::ptrdiff_t least_max_value>
void CountingSemaphore<least_max_value>::destroy(CountingSemaphore& obj) noexcept
{
    if (obj.handle_ != nullptr) {
        vSemaphoreDelete(obj.handle_);
        obj.handle_ = nullptr;
    }
}

template<std::ptrdiff_t least_max_value>
void CountingSemaphore<least_max_value>::release(bool in_isr) noexcept
{
    if (in_isr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        xSemaphoreGiveFromISR(handle_, &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else {
        xSemaphoreGive(handle_);
    }
}

template<std::ptrdiff_t least_max_value>
ErrorCode CountingSemaphore<least_max_value>::acquire(bool in_isr, SysTick_t timeout_tick) noexcept
{
    if (in_isr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        auto ret = xSemaphoreTakeFromISR(handle_, &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        return (ret == pdTRUE ? ErrorCode::Success : ErrorCode::OperationFail);
    }
    else {
        auto ret = xSemaphoreTake(static_cast<SemaphoreHandle_t>(handle_), timeout_tick);

        return (ret == pdTRUE ? ErrorCode::Success : ErrorCode::Timeout);
    }
}

template<std::ptrdiff_t least_max_value>
CountingSemaphore<least_max_value>::~CountingSemaphore() noexcept
{
    if (handle_ != nullptr) {
        this->destroy();
    }
}

}  // namespace emdevif

// for template<> CountingSemaphore<1>
namespace emdevif {

template<>
class CountingSemaphore<1>
{
public:
    using Handle = void*;

public:
    explicit CountingSemaphore(CountingSemaphoreBuilder builder) noexcept;

    static void destroy(CountingSemaphore& obj) noexcept;

    void destroy() noexcept
    {
        destroy(*this);
        handle_ = nullptr;
    }

    void release(bool in_isr) noexcept;

    ErrorCode acquire(bool in_isr, SysTick_t timeout_tick = Thread::maxDelay()) noexcept;

    ErrorCode try_acquire(const bool in_isr) noexcept
    {
        return acquire(in_isr, 0U);
    }

    [[nodiscard]] Handle getHandle() const noexcept
    {
        return handle_;
    }

    CountingSemaphore() noexcept : handle_(nullptr) {}

    CountingSemaphore& operator=(const CountingSemaphore&) = delete;
    CountingSemaphore(const CountingSemaphore&) = delete;

    CountingSemaphore(CountingSemaphore&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}

    CountingSemaphore& operator=(CountingSemaphore&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        if (handle_ != nullptr) {
            EMDEVIF_FATAL_HANDLER("Should not move-assign to a non-empty binary semaphore!");
            return *this;
        }

        this->handle_ = std::exchange(other.handle_, nullptr);

        return *this;
    }

    ~CountingSemaphore() noexcept;

private:
    Handle handle_;
};

inline void CountingSemaphore<1>::release(bool in_isr) noexcept
{
    if (in_isr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        xSemaphoreGiveFromISR(handle_, &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else {
        xSemaphoreGive(handle_);
    }
}

inline ErrorCode CountingSemaphore<1>::acquire(bool in_isr, SysTick_t timeout_tick) noexcept
{
    if (in_isr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        auto ret = xSemaphoreTakeFromISR(handle_, &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        return (ret == pdTRUE ? ErrorCode::Success : ErrorCode::OperationFail);
    }
    else {
        auto ret = xSemaphoreTake(static_cast<SemaphoreHandle_t>(handle_), timeout_tick);

        return (ret == pdTRUE ? ErrorCode::Success : ErrorCode::Timeout);
    }
}

}  // namespace emdevif

#endif  // !EMDEVIF_FREERTOS_SYSTEM_IMPL_SEMAPHORE_INL
