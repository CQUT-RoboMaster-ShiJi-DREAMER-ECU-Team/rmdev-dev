/**
 * @file resource_guard.cppm
 * @brief RAII 资源管理
 */

module;

export module emdevif.core.resource_guard;

export import emdevif.core.resource_guard.lock_guard;
export import emdevif.core.resource_guard.init_guard;
export import emdevif.core.resource_guard.defer;
export import emdevif.core.resource_guard.try_finally;
