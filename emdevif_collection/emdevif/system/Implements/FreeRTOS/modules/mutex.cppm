/**
 * @file mutex.cppm
 * @brief 互斥锁
 */

module;

#if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#else
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#include <cstddef>

#include "emdevif/system/mutex.hpp"

export module emdevif.system.mutex;

export namespace emdevif {
using ::emdevif::Mutex;
using ::emdevif::MutexBuilder;
using ::emdevif::MutexStaticInstance;
}  // namespace emdevif
