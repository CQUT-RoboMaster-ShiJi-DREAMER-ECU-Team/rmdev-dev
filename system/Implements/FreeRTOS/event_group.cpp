/**
 * @file event_group.cpp
 * @brief
 */

#if (defined(EMDEVIF_USE_MODULES) && EMDEVIF_USE_MODULES)
module;
#else
#include "emdevif/core/error_handler.hpp"
#include "emdevif/system/event_group.hpp"
#endif

#include <cstddef>

#if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#else
#include "FreeRTOS.h"
#include "event_groups.h"
#endif

#if (defined(EMDEVIF_USE_MODULES) && EMDEVIF_USE_MODULES)
module emdevif.system.event_group;

import emdevif.core.error_handler;
#endif

namespace emdevif {

EventGroup::EventGroup(EventGroupBuilder builder) noexcept : handle_(nullptr)
{
    Handle handle;

    if (builder.static_instance != nullptr) {
        auto& static_instance = static_cast<StaticEventGroup_t&>(*builder.static_instance);

        handle = xEventGroupCreateStatic(&static_instance);
    }
    else {
        handle = xEventGroupCreate();
    }

    handle_ = handle;
}

void EventGroup::destroy(EventGroup& obj) noexcept
{
    if (obj.handle_ != nullptr) {
        vEventGroupDelete(static_cast<EventGroupHandle_t>(obj.handle_));

        obj.handle_ = nullptr;
    }
}

EventGroup::~EventGroup()
{
    if (handle_ != nullptr) {
        this->destroy();
    }
}

}  // namespace emdevif
