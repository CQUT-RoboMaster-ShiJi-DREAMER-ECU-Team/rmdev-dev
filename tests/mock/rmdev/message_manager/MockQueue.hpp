#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string_view>
#include "emdevif/core/error_handler.hpp"
#include "emdevif/core/data_container/message_queue.hpp"

template<typename T, size_t N=8>
struct MockQueue {
    using ValueType = T;
    static constexpr size_t item_size = N;
    struct Config { std::string_view name; };
    std::vector<T> data;
    static MockQueue create(const Config&) noexcept { return {}; }
    static void destroy(MockQueue&) noexcept {}
    void destroy() noexcept { data.clear(); }
    emdevif::ErrorCode push(bool, const T& val, emdevif::MessageQueueTimeout_t=0) noexcept {
        if(data.size()>=item_size) return emdevif::ErrorCode::Full;
        data.push_back(val); return emdevif::ErrorCode::Success;
    }
    emdevif::ErrorCode pop(bool, T& val, emdevif::MessageQueueTimeout_t=0) noexcept {
        if(data.empty()) return emdevif::ErrorCode::Empty;
        val=data.front(); data.erase(data.begin()); return emdevif::ErrorCode::Success;
    }
    emdevif::ErrorCode pop(bool) noexcept {
        if(data.empty()) return emdevif::ErrorCode::Empty;
        data.erase(data.begin()); return emdevif::ErrorCode::Success;
    }
    emdevif::ErrorCode forcePush(bool, const T& val) noexcept { data.push_back(val); return emdevif::ErrorCode::Success; }
    emdevif::ErrorCode peek(bool, T& val, emdevif::MessageQueueTimeout_t=0) noexcept {
        if(data.empty()) return emdevif::ErrorCode::Empty;
        val=data.front(); return emdevif::ErrorCode::Success;
    }
    void clear() noexcept { data.clear(); }
    size_t storeCount() const noexcept { return data.size(); }
    size_t remainCount() const noexcept { return item_size-data.size(); }
    static constexpr size_t maxItemCount() noexcept { return item_size; }
    void* getHandle() const noexcept { return nullptr; }
};

template<typename T>
struct MockSlot {
    using ValueType = T;
    static constexpr size_t item_size = 1;
    struct Config { std::string_view name; };
    T value_{}; bool has_data_=false;
    static MockSlot create(const Config&) noexcept { return {}; }
    static void destroy(MockSlot&) noexcept {}
    void destroy() noexcept { has_data_=false; }
    emdevif::ErrorCode forcePush(bool, const T& val) noexcept { value_=val; has_data_=true; return emdevif::ErrorCode::Success; }
    emdevif::ErrorCode peek(bool, T& val, emdevif::MessageQueueTimeout_t=0) noexcept {
        if(!has_data_) return emdevif::ErrorCode::Empty;
        val=value_; return emdevif::ErrorCode::Success;
    }
    void clear() noexcept { has_data_=false; }
    size_t storeCount() const noexcept { return has_data_?1u:0u; }
    size_t remainCount() const noexcept { return has_data_?0u:1u; }
    static constexpr size_t maxItemCount() noexcept { return item_size; }
    void* getHandle() const noexcept { return nullptr; }
};
