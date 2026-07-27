/**
 * @file defer.cppm
 * @brief 延迟执行守卫
 */

module;

#include "emdevif/core/resource_guard/defer.hpp"

export module emdevif.core.resource_guard.defer;

export namespace emdevif {
using ::emdevif::Defer;
using ::emdevif::DeferFunction;
}  // namespace emdevif
