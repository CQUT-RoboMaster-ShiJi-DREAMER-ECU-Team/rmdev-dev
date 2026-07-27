/**
 * @file event_group_definitions.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_FREERTOS_SYSTEM_IMPL_EVENT_GROUP_DEFINITIONS_HPP
#define EMDEVIF_FREERTOS_SYSTEM_IMPL_EVENT_GROUP_DEFINITIONS_HPP

#include <cstdint>

#if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#else
#include "FreeRTOS.h"
#include "event_groups.h"
#endif

#include "emdevif/core/utils/bit_int.hpp"

namespace emdevif::detail {

/**
 * @brief 获取当前 FreeRTOS 配置下事件组支持的最大位数
 * @return 最大位数。当 configUSE_16_BIT_TICKS 为 true 时返回 8，否则返回 24
 */
consteval BitsType_t maxEventBits() noexcept
{
#if (configUSE_16_BIT_TICKS)
    return 8;
#else
    return 24;
#endif
}

}  // namespace emdevif::detail

#endif  // !EMDEVIF_FREERTOS_SYSTEM_IMPL_EVENT_GROUP_DEFINITIONS_HPP
