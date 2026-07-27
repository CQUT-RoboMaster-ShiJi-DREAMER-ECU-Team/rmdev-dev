/**
 * @file event_group_implements.inl
 * @brief
 */

#pragma once
#ifndef EMDEVIF_FREERTOS_SYSTEM_IMPL_EVENT_GROUP_IMPLEMENTS_INL
    #define EMDEVIF_FREERTOS_SYSTEM_IMPL_EVENT_GROUP_IMPLEMENTS_INL

    #if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
        #include "freertos/FreeRTOS.h"
        #include "freertos/event_groups.h"
    #else
        #include "FreeRTOS.h"
        #include "event_groups.h"
    #endif

    #include "emdevif/core/utils/bit_int.hpp"

    #include <cstdint>

    #include <concepts>

namespace emdevif {

class EventGroupStaticInstance
{
public:
    EventGroupStaticInstance() noexcept : static_instance() {}

    explicit operator StaticEventGroup_t&()
    {
        return static_instance;
    }

private:
    StaticEventGroup_t static_instance;
};

inline auto EventGroup::getBits(const bool in_isr) noexcept -> EventGroup::EventBits_t
{
    ::EventBits_t ret;

    if (in_isr) {
        ret = xEventGroupGetBitsFromISR(static_cast<EventGroupHandle_t>(handle_));
    }
    else {
        ret = xEventGroupGetBits(static_cast<EventGroupHandle_t>(handle_));
    }

    return EventGroup::EventBits_t(ret);
}

inline ErrorCode EventGroup::setBits(const bool in_isr, const EventBits_t bits) noexcept
{
    if (in_isr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        const auto status =
            xEventGroupSetBitsFromISR(static_cast<EventGroupHandle_t>(handle_), bits, &xHigherPriorityTaskWoken);

        if (status == pdPASS) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        else {
            return ErrorCode::OperationFail;
        }
    }
    else {
        xEventGroupSetBits(static_cast<EventGroupHandle_t>(handle_), bits);
    }

    return ErrorCode::Success;
}

inline ErrorCode EventGroup::clearBits(const bool in_isr, const EventBits_t bits) noexcept
{
    if (in_isr) {
        const auto status = xEventGroupClearBitsFromISR(static_cast<EventGroupHandle_t>(handle_), bits);

        if (status == pdTRUE) {
            portYIELD_FROM_ISR(pdTRUE);
        }
        else {
            return ErrorCode::OperationFail;
        }
    }
    else {
        xEventGroupClearBits(static_cast<EventGroupHandle_t>(handle_), bits);
    }

    return ErrorCode::Success;
}

inline ErrorCode EventGroup::waitBits(const EventBits_t bits_wait_for,
                                      const bool clear_on_exit,
                                      const bool wait_for_all_bits,
                                      const SysTick_t timeout_tick) noexcept
{
    const auto ret = xEventGroupWaitBits(static_cast<EventGroupHandle_t>(handle_),
                                         bits_wait_for,
                                         clear_on_exit,
                                         wait_for_all_bits,
                                         timeout_tick);

    if (wait_for_all_bits) {
        if ((ret & bits_wait_for) == bits_wait_for) {
            return ErrorCode::Success;
        }
    }
    else {
        if ((ret & bits_wait_for) != 0) {
            return ErrorCode::Success;
        }
    }

    return ErrorCode::Timeout;
}

inline ErrorCode EventGroup::sync(const EventBits_t bits_to_set,
                                  const EventBits_t bits_wait_for,
                                  const SysTick_t timeout_tick) noexcept
{
    const auto ret =
        xEventGroupSync(static_cast<EventGroupHandle_t>(handle_), bits_to_set, bits_wait_for, timeout_tick);

    if ((ret & bits_wait_for) == bits_wait_for) {
        return ErrorCode::Success;
    }

    return ErrorCode::Timeout;
}

}  // namespace emdevif

#endif  // !EMDEVIF_FREERTOS_SYSTEM_IMPL_EVENT_GROUP_IMPLEMENTS_INL
