/**
 * @file event_group.cppm
 * @brief 事件组接口
 */

module;

#if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#else
#include "FreeRTOS.h"
#include "event_groups.h"
#endif

#include "emdevif/system/event_group.hpp"

export module emdevif.system.event_group;

export namespace emdevif {
using ::emdevif::EventGroup;
using ::emdevif::EventGroupBuilder;
using ::emdevif::EventGroupStaticInstance;
}  // namespace emdevif
