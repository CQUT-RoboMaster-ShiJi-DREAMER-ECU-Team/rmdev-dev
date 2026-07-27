#if EMDEVIF_USE_MODULES
module;

module emdevif.logger;

import emdevif.core.error_handler;
import emdevif.core.data_container.ring_buffer;
import emdevif.core.resource_guard.lock_guard;
import emdevif.system.mutex;
import emdevif.system.semaphore;
import emdevif.system.thread;
#else
#include "emdevif/logger.hpp"
#include "emdevif/core/error_handler.hpp"
#include "emdevif/core/data_container/ring_buffer.hpp"
#include "emdevif/core/resource_guard/lock_guard.hpp"
#include "emdevif/system/mutex.hpp"
#include "emdevif/system/semaphore.hpp"
#include "emdevif/system/thread.hpp"
#include "emdevif/core/attributes_and_useful_macros.h"
#include "emdevif/core/fatal_handler.h"
#include "emdevif/core/line_separator.h"
#endif

namespace emdevif::logger::detail::async {

static VsnprintfImpl vsnprintf_impl_ = nullptr;

static int snprintf_(char* dest, const std::size_t buffer_size, const char* format, ...) noexcept
{
    std::va_list args;
    va_start(args, format);
    const auto ret = vsnprintf_impl_(dest, buffer_size, format, args);
    va_end(args);
    return ret;
}

static constexpr auto logger_async_thread_stack_size = static_cast<std::size_t>(EMDEVIF_LOGGER_ASYNC_THREAD_STACK_SIZE);

static RingBuffer<std::array<char, logger_buffer_size>, logger_buffer_count> buffer_{};

static Mutex mutex_;
#if (!EMDEVIF_LOGGER_DYNAMIC_CREATE)
static MutexStaticInstance mutex_static_instance_;
#endif

static Thread logger_async_printer_thread_;
#if (!EMDEVIF_LOGGER_DYNAMIC_CREATE)
static ThreadStaticInstance<logger_async_thread_stack_size> logger_async_printer_thread_instance_;
#endif

static BinarySemaphore logger_async_printer_semaphore_;
#if (!EMDEVIF_LOGGER_DYNAMIC_CREATE)
static CountingSemaphoreStaticInstance<1> logger_async_printer_semaphore_static_instance_;
#endif

EMDEVIF_NO_RETURN static void logPrinterThread(void*) noexcept
{
    logger_async_printer_semaphore_.release(false);

    while (true) {
        logger_async_printer_semaphore_.acquire(false);

        mutex_.lock();

        if (!buffer_.isEmpty()) {
            if (::emdevif::user_impl::logger::printLogMessage(&buffer_.peekRef()[0]) == ErrorCode::Success) {
                buffer_.discard(1);
            }

            mutex_.unlock();
            logger_async_printer_semaphore_.release(false);

            Thread::yield();
        }
        else {
            mutex_.unlock();
        }
    }
}

ErrorCode logInit(const VsnprintfImpl vsprintf_impl) noexcept
{
    vsnprintf_impl_ = vsprintf_impl;

    buffer_.clear();

    mutex_ = Mutex{MutexBuilder{
        .name = "loggerMutex",
#if (!EMDEVIF_LOGGER_DYNAMIC_CREATE)
        .static_instance = &mutex_static_instance_,
#endif
    }};
    if (mutex_.getHandle() == nullptr) {
        return ErrorCode::OperationFail;
    }

    logger_async_printer_semaphore_ = BinarySemaphore{CountingSemaphoreBuilder{
        .name = "loggerSemaphore",
#if (!EMDEVIF_LOGGER_DYNAMIC_CREATE)
        .static_instance = &logger_async_printer_semaphore_static_instance_,
#endif
    }};
    if (logger_async_printer_semaphore_.getHandle() == nullptr) {
        return ErrorCode::OperationFail;
    }

    logger_async_printer_thread_ = Thread::create(
        {
            .name = "loggerThread",
            .priority = ThreadPriority::Low,
#if (!EMDEVIF_LOGGER_DYNAMIC_CREATE)
            .static_instance = logger_async_printer_thread_instance_.getInstanceAddr(),
            .stack_size = logger_async_printer_thread_instance_.getStackDepth(),
#else
            .stack_size = logger_async_thread_stack_size,
#endif
        },
        logPrinterThread,
        nullptr);
    if (logger_async_printer_thread_.getHandle() == nullptr) {
        return ErrorCode::OperationFail;
    }

    return ErrorCode::Success;
}

void logDeInit() noexcept
{
#if (!EMDEVIF_LOGGER_DYNAMIC_CREATE)
    EMDEVIF_FATAL_HANDLER(
        "emdevif_logger was created with static allocation, so it should not be destroyed. If you want to destroy it, "
        "please use dynamic create(set macro `EMDEVIF_LOGGER_DYNAMIC_CREATE` to true).");
#else
    mutex_.destroy();
    logger_async_printer_semaphore_.destroy();
    Thread::destroy(logger_async_printer_thread_);
#endif
}

void logImpl(const LoggerLevel level, const char* format, std::va_list args) noexcept
{
    {
        const LockGuard lock_guard{lock_guard_do_not_lock_when_init, mutex_};
        lock_guard.lock().terminateIfNotSucceed();

        auto* const p_buffer = buffer_.nextSlot().data();
        const auto logMsgBufferLength = [] { return logger_buffer_size; };

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

        if (level == LoggerLevel::Fatal) {
            static std::uint8_t fail_count = 0;
            while (!buffer_.isEmpty()) {
                if (::emdevif::user_impl::logger::printLogMessage(&buffer_.peekRef()[0]) == ErrorCode::Success) {
                    buffer_.discard(1);
                    fail_count = 0;
                }
                else {
                    if (fail_count >= 3) {
                        fail_count = 0;
                        break;
                    }
                    ++fail_count;
                }
            }

            terminate();
        }

        buffer_.useNextSlot();
    }

    logger_async_printer_semaphore_.release(false);
}

}  // namespace emdevif::logger::detail::async