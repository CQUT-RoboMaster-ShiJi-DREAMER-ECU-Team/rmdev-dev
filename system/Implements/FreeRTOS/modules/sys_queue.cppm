/**
 * @file sys_queue.cppm
 * @brief 系统队列
 */

module;

#if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#else
#include "FreeRTOS.h"
#include "queue.h"
#endif

#include "emdevif/system/sys_queue.hpp"

export module emdevif.system.sys_queue;

export namespace emdevif {
using ::emdevif::SysQueue;
using ::emdevif::SysQueueBuilder;
using ::emdevif::SysQueueStaticInstance;
}  // namespace emdevif
