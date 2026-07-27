/**
 * @file mutex.inl
 * @brief
 */

#pragma once
#ifndef EMDEVIF_FREERTOS_SYSTEM_IMPL_MUTEX_INL
    #define EMDEVIF_FREERTOS_SYSTEM_IMPL_MUTEX_INL

    #include "emdevif/core/detail/config.hpp"

    #if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
        #include "freertos/FreeRTOS.h"
        #include "freertos/semphr.h"
    #else
        #include "FreeRTOS.h"
        #include "semphr.h"
    #endif

    #include "emdevif/core/fatal_handler.h"

    #include "emdevif/core/error_handler.hpp"

    #include <cstddef>

namespace emdevif {

class MutexStaticInstance
{
public:
    MutexStaticInstance() noexcept : instance() {}

    friend class Mutex;

private:
    explicit operator StaticSemaphore_t&() noexcept
    {
        return instance;
    }

private:
    StaticSemaphore_t instance;
};

inline ErrorCode Mutex::lock(const SysTick_t timeout_tick) noexcept
{
    const auto ret = xSemaphoreTake(static_cast<SemaphoreHandle_t>(handle_), timeout_tick);

    if (ret == pdTRUE) {
        return ErrorCode::Success;
    }
    else {
        return ErrorCode::Timeout;
    }
}

inline void Mutex::unlock() noexcept
{
    if (xSemaphoreGive(handle_) != pdTRUE) {
        EMDEVIF_FATAL_HANDLER("Failed to Unlock mutex!");
    }
}

}  // namespace emdevif

#endif  // !EMDEVIF_FREERTOS_SYSTEM_IMPL_MUTEX_INL
