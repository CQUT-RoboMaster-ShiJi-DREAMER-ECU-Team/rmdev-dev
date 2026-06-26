#pragma once

#include <cstddef>
#include <cstdint>

#include <queue>
#include <string_view>

#if EMDEVIF_USE_MODULES
import emdevif.core.error_handler;
#else
#include "emdevif/core/error_handler.hpp"
#endif
#if EMDEVIF_USE_MODULES
import emdevif.core.data_container.message_queue;
#else
#include "emdevif/core/data_container/message_queue.hpp"
#endif

template<typename T, size_t N = 8>
class MockQueue
{
public:
    using ValueType = T;
    static constexpr size_t item_size = N;

    struct Config {
        std::string_view name;
    };

    MockQueue() = default;

    MockQueue(const MockQueue&) = delete;
    MockQueue& operator=(const MockQueue&) = delete;

    MockQueue(MockQueue&&) = default;
    MockQueue& operator=(MockQueue&&) = default;

    explicit MockQueue(Config) noexcept {}

private:
    std::queue<T> data;

public:
    static MockQueue create(const Config& config) noexcept
    {
        return MockQueue{Config{config}};
    }

    static void destroy(MockQueue&) noexcept {}

    void destroy() noexcept
    {
        data = std::queue<T>{};
    }

    emdevif::ErrorCode push(bool, const T& val, emdevif::MessageQueueTimeout_t = 0) noexcept
    {
        if (data.size() >= item_size) {
            return emdevif::ErrorCode::Full;
        }
        data.push(val);
        return emdevif::ErrorCode::Success;
    }

    emdevif::ErrorCode pop(bool, T& val, emdevif::MessageQueueTimeout_t = 0) noexcept
    {
        if (data.empty()) {
            return emdevif::ErrorCode::Empty;
        }
        val = data.front();
        data.pop();
        return emdevif::ErrorCode::Success;
    }

    emdevif::ErrorCode pop(bool) noexcept
    {
        if (data.empty()) {
            return emdevif::ErrorCode::Empty;
        }
        data.pop();
        return emdevif::ErrorCode::Success;
    }

    emdevif::ErrorCode forcePush(bool, const T& val) noexcept
    {
        if (data.size() < item_size) {
            data.push(val);
        }
        else {
            data.front() = val;
        }
        return emdevif::ErrorCode::Success;
    }

    emdevif::ErrorCode peek(bool, T& val, emdevif::MessageQueueTimeout_t = 0) noexcept
    {
        if (data.empty()) {
            return emdevif::ErrorCode::Empty;
        }
        val = data.front();
        return emdevif::ErrorCode::Success;
    }

    void clear() noexcept
    {
        data = std::queue<T>{};
    }

    size_t storeCount() const noexcept
    {
        return data.size();
    }

    size_t remainCount() const noexcept
    {
        return item_size - data.size();
    }

    static constexpr size_t maxItemCount() noexcept
    {
        return item_size;
    }

    void* getHandle() const noexcept
    {
        return nullptr;
    }
};

template<typename T>
class MockSlot
{
public:
    using ValueType = T;
    static constexpr size_t item_size = 1;

    struct Config {
        std::string_view name;
    };

    MockSlot() = default;

    MockSlot(const MockSlot&) = delete;
    MockSlot& operator=(const MockSlot&) = delete;

    MockSlot(MockSlot&&) = default;
    MockSlot& operator=(MockSlot&&) = default;

private:
    MockQueue<T, 1> queue_;

public:
    static MockSlot create(const Config& cfg) noexcept
    {
        MockSlot s;
        typename MockQueue<T, 1>::Config qcfg{cfg.name};
        s.queue_ = MockQueue<T, 1>::create(qcfg);
        return s;
    }

    static void destroy(MockSlot&) noexcept {}

    void destroy() noexcept
    {
        queue_.destroy();
    }

    emdevif::ErrorCode forcePush(bool in_isr, const T& val) noexcept
    {
        return queue_.forcePush(in_isr, val);
    }

    emdevif::ErrorCode peek(bool in_isr, T& val, emdevif::MessageQueueTimeout_t = 0) noexcept
    {
        return queue_.peek(in_isr, val);
    }

    void clear() noexcept
    {
        queue_.clear();
    }

    size_t storeCount() const noexcept
    {
        return queue_.storeCount();
    }

    size_t remainCount() const noexcept
    {
        return queue_.remainCount();
    }

    static constexpr size_t maxItemCount() noexcept
    {
        return item_size;
    }

    void* getHandle() const noexcept
    {
        return nullptr;
    }
};
