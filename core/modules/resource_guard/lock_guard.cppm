/**
 * @file lock_guard.cppm
 * @brief 互斥锁封装器
 */

module;

#include "emdevif/core/resource_guard/lock_guard.hpp"

export module emdevif.core.resource_guard.lock_guard;

export namespace emdevif {
using ::emdevif::lock_guard_do_not_lock_when_init;
using ::emdevif::LockGuard;
using ::emdevif::LockGuardDoNotLockWhenInit;
using ::emdevif::ValidLock;
}  // namespace emdevif
