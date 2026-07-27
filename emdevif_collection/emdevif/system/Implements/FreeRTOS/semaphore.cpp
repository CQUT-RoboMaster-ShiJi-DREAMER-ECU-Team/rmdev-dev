/**
 * @file semaphore.cpp
 * @brief
 */

#if (defined(EMDEVIF_USE_MODULES) && EMDEVIF_USE_MODULES)
module;
#else
#include "emdevif/core/error_handler.hpp"
#include "emdevif/system/semaphore.hpp"
#endif

#include <cstddef>

#if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#else
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#include "emdevif/core/fatal_handler.h"

#if (defined(EMDEVIF_USE_MODULES) && EMDEVIF_USE_MODULES)
module emdevif.system.semaphore;

import emdevif.core.error_handler;
#endif

namespace emdevif {

/**
 * @brief 通过 Builder 构造信号量（FreeRTOS 实现，二值信号量）
 * @param builder Builder
 */
CountingSemaphore<1>::CountingSemaphore(CountingSemaphoreBuilder builder) noexcept : handle_(nullptr)
{
    Handle handle = nullptr;

    if (builder.static_instance != nullptr) {
        auto& static_instance = *static_cast<CountingSemaphoreStaticInstance<1>*>(builder.static_instance);

        handle = xSemaphoreCreateBinaryStatic(&static_cast<StaticSemaphore_t&>(static_instance));
    }
    else {
        handle = xSemaphoreCreateBinary();
    }

#if (configQUEUE_REGISTRY_SIZE > 0)
    if (handle != nullptr) {
        vQueueAddToRegistry(static_cast<QueueHandle_t>(handle), builder.name);
    }
#endif

    handle_ = handle;
}

/**
 * @brief 销毁信号量并释放底层 FreeRTOS 信号量资源
 * @param obj 待销毁的信号量
 */
void CountingSemaphore<1>::destroy(CountingSemaphore<1>& obj) noexcept
{
    if (obj.handle_ != nullptr) {
        vSemaphoreDelete(obj.handle_);
        obj.handle_ = nullptr;
    }
}

/**
 * @brief 析构函数，自动销毁信号量
 */
CountingSemaphore<1>::~CountingSemaphore() noexcept
{
    if (handle_ != nullptr) {
        this->destroy();
    }
}

}  // namespace emdevif
