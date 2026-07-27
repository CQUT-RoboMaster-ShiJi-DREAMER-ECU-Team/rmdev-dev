/**
 * @file init_guard.cppm
 * @brief RAII 风格的 init/deInit 自动管理
 */

module;

#include "emdevif/core/resource_guard/init_guard.hpp"

export module emdevif.core.resource_guard.init_guard;

export namespace emdevif {
using ::emdevif::init_guard_do_not_init_object_tag;
using ::emdevif::InitGuard;
using ::emdevif::InitGuardDoNotInitObjectTag;
using ::emdevif::ValidHaveInitDeInitPairObject;
}  // namespace emdevif
