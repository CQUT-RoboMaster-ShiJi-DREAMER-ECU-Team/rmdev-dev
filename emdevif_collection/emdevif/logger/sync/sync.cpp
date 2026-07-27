#if EMDEVIF_USE_MODULES
module;

module emdevif.logger;

import emdevif.core.error_handler;

#if (defined(EMDEVIF_LOGGER_SYNC_USE_LOCK) && EMDEVIF_LOGGER_SYNC_USE_LOCK)
import emdevif.core.resource_guard.lock_guard;
import emdevif.system.mutex;
#endif
#else
#include "emdevif/logger.hpp"
#include "emdevif/core/error_handler.hpp"
#include "emdevif/core/attributes_and_useful_macros.h"
#include "emdevif/core/fatal_handler.h"
#include "emdevif/core/line_separator.h"
#if (defined(EMDEVIF_LOGGER_SYNC_USE_LOCK) && EMDEVIF_LOGGER_SYNC_USE_LOCK)
#include "emdevif/core/resource_guard/lock_guard.hpp"
#include "emdevif/system/mutex.hpp"
#endif
#endif

namespace emdevif::logger::detail::sync {

static VsnprintfImpl vsnprintf_impl_ = nullptr;

static int snprintf_(char* dest, const std::size_t buffer_size, const char* format, ...) noexcept
{
    std::va_list args;
    va_start(args, format);
    const auto ret = vsnprintf_impl_(dest, buffer_size, format, args);
    va_end(args);
    return ret;
}

static char log_msg_buffer_[logger_buffer_count][logger_buffer_size];

static constexpr std::size_t logMsgBufferLength() noexcept
{
    return logger_buffer_count * logger_buffer_size;
}

#if (defined(EMDEVIF_LOGGER_SYNC_USE_LOCK) && EMDEVIF_LOGGER_SYNC_USE_LOCK)
static emdevif::Mutex logger_mutex_;

#if (!EMDEVIF_LOGGER_DYNAMIC_CREATE)
static emdevif::MutexStaticInstance logger_mutex_static_instance_;
#endif
#endif

ErrorCode logInit(const VsnprintfImpl vsprintf_impl) noexcept
{
    vsnprintf_impl_ = vsprintf_impl;

#if (defined(EMDEVIF_LOGGER_SYNC_USE_LOCK) && EMDEVIF_LOGGER_SYNC_USE_LOCK)
    logger_mutex_ = emdevif::Mutex{emdevif::MutexBuilder{
        .name = "loggerMutex",
#if (!EMDEVIF_LOGGER_DYNAMIC_CREATE)
        .static_instance = &logger_mutex_static_instance_,
#endif
    }};
#endif
    return ErrorCode::Success;
}

void logDeInit() noexcept
{
#if (EMDEVIF_LOGGER_DYNAMIC_CREATE)
    logger_mutex_.destroy();
#else
    EMDEVIF_FATAL_HANDLER(
        "emdevif_logger was created with static allocation, so it should not be destroyed. If you want to destroy it, "
        "please use dynamic create(set macro `EMDEVIF_LOGGER_DYNAMIC_CREATE` to true).");
#endif

    vsnprintf_impl_ = nullptr;
}

void logImpl(const LoggerLevel level, const char* format, std::va_list args) noexcept
{
#if (defined(EMDEVIF_LOGGER_SYNC_USE_LOCK) && EMDEVIF_LOGGER_SYNC_USE_LOCK)
    const LockGuard lock_guard{lock_guard_do_not_lock_when_init, logger_mutex_};
    lock_guard.lock().terminateIfNotSucceed();
#endif

    auto* const p_buffer = reinterpret_cast<char*>(log_msg_buffer_);

    const auto index1 = snprintf_(p_buffer,
                                  logMsgBufferLength(),
                                  "%-10zu %7s ",
                                  ::emdevif::user_impl::logger::getTimeLine(),
                                  toCString(level));
    if (index1 < 0) {
        return;
    }

    const auto index2 = vsnprintf_impl_(p_buffer + index1, logMsgBufferLength() - index1, format, args);
    if (index2 < 0) {
        return;
    }

    const std::size_t msg_len = index1 + index2;

#if (EMDEVIF_LINE_SEPARATOR_MODE == EMDEVIF_LINE_SEPARATOR_MODE_LF)
    if (msg_len + 1 < logMsgBufferLength()) {
        p_buffer[msg_len] = '\n';
        p_buffer[msg_len + 1] = '\0';
    }
    else {
        p_buffer[logMsgBufferLength() - 1] = '\0';
    }
#elif (EMDEVIF_LINE_SEPARATOR_MODE == EMDEVIF_LINE_SEPARATOR_MODE_CR)
    if (msg_len + 1 < logMsgBufferLength()) {
        p_buffer[msg_len] = '\r';
        p_buffer[msg_len + 1] = '\0';
    }
    else {
        p_buffer[logMsgBufferLength() - 1] = '\0';
    }
#else
    if (msg_len + 2 < logMsgBufferLength()) {
        p_buffer[msg_len] = '\r';
        p_buffer[msg_len + 1] = '\n';
        p_buffer[msg_len + 2] = '\0';
    }
    else {
        p_buffer[logMsgBufferLength() - 1] = '\0';
    }
#endif

    ::emdevif::user_impl::logger::printLogMessage(p_buffer);
}

}  // namespace emdevif::logger::detail::sync