/**
 * @file semaphore.cppm
 * @brief 信号量接口
 */

module;

#if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#else
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#include "emdevif/system/semaphore.hpp"

export module emdevif.system.semaphore;

export namespace emdevif {
using ::emdevif::BinarySemaphore;
using ::emdevif::CountingSemaphore;
using ::emdevif::CountingSemaphoreBuilder;
using ::emdevif::CountingSemaphoreStaticInstance;
}  // namespace emdevif
