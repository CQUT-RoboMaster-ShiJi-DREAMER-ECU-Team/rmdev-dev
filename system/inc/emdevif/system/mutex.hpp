/**
 * @file mutex.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_SYSTEM_MUTEX_HPP
#define EMDEVIF_SYSTEM_MUTEX_HPP

#include <cstdint>
#include <utility>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/system/thread.hpp"

namespace emdevif {

/**
 * 互斥锁的静态实例
 * @copydoc sys_static_instance
 */
class MutexStaticInstance;

/**
 * 互斥锁 Builder
 */
struct MutexBuilder {
    const char* name{};                             ///< 名称

    MutexStaticInstance* static_instance{nullptr};  ///< 静态实例内存
};

/**
 * 互斥锁
 */
class Mutex
{
public:
    using Handle = void*;  ///< 底层实现的句柄

public:
    // todo 这里的注释中的示例仿照 thread 里的来写

    /**
     * 通过 Builder 构造互斥锁
     * @param builder Builder
     */
    explicit Mutex(MutexBuilder builder) noexcept;

    /**
     * 销毁互斥锁
     * @param obj 待销毁的互斥锁
     */
    static void destroy(Mutex& obj) noexcept;

    /**
     * 销毁互斥锁（自身）
     */
    void destroy() noexcept
    {
        destroy(*this);
        handle_ = nullptr;
    }

    /**
     * 获取互斥锁，如果已经被占用，则等待
     * @param timeout_tick 等待时间，默认无限等待
     * @retval ErrorCode::Success 成功锁定
     * @retval ErrorCode::Timeout 等待超时
     */
    ErrorCode lock(SysTick_t timeout_tick = Thread::maxDelay()) noexcept;

    /**
     * 尝试获取互斥锁（不会等待。如果失败，立刻返回）
     * @retval ErrorCode::Success 成功锁定
     * @retval ErrorCode::Timeout 等待超时
     */
    [[nodiscard]] ErrorCode try_lock() noexcept
    {
        return lock(0U);
    }

    /**
     * 释放互斥锁
     */
    void unlock() noexcept;

    /**
     * 获取底层实现的句柄
     * @return 底层实现的句柄
     */
    [[nodiscard]] Handle getHandle() const noexcept
    {
        return handle_;
    }

    Mutex() noexcept : handle_(nullptr) {}

    Mutex& operator=(const Mutex&) = delete;
    Mutex(const Mutex&) = delete;

    Mutex(Mutex&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}

    Mutex& operator=(Mutex&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        if (handle_ != nullptr) {
            EMDEVIF_FATAL_HANDLER("Should not move-assign to a non-empty mutex!");
            return *this;
        }

        this->handle_ = std::exchange(other.handle_, nullptr);

        return *this;
    }

    ~Mutex() noexcept;

private:
    Handle handle_;  ///< 底层实现的句柄
};

}  // namespace emdevif

#include "emdevif/system_impl/mutex.inl"

#endif  // !EMDEVIF_SYSTEM_MUTEX_HPP
