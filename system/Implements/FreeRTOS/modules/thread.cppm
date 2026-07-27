/**
 * @file thread.cppm
 * @brief 线程接口
 */

module;

#if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#else
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "emdevif/core/attributes_and_useful_macros.h"
#include "emdevif/core/fatal_handler.h"
#include "emdevif/system/thread.hpp"

export module emdevif.system.thread;

export namespace emdevif {
using ::emdevif::SysTick_t;
using ::emdevif::Thread;
using ::emdevif::ThreadBuilder;
using ::emdevif::ThreadEntry;
using ::emdevif::ThreadPriority;
using ::emdevif::ThreadStaticInstance;
}  // namespace emdevif
