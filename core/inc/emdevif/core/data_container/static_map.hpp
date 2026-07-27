/**
 * @file static_map.hpp
 * @brief 编译期静态键值对
 */

#pragma once
#ifndef EMDEVIF_CORE_DATA_CONTAINER_STATIC_MAP_HPP
#define EMDEVIF_CORE_DATA_CONTAINER_STATIC_MAP_HPP

#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <utility>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"

namespace emdevif {

/**
 * 静态键值对
 * @tparam KeyType 键的类型
 * @tparam ValueType 值的类型
 * @tparam max_size 键值对最大数量
 */
template<typename KeyType, typename ValueType, std::size_t max_size, typename KeyEqual = std::equal_to<KeyType>>
class StaticMap
{
public:
    static_assert(max_size > 0, "The max_size must be greater than 0");

    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<key_type, mapped_type>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = typename std::array<value_type, max_size>::iterator;
    using const_iterator = typename std::array<value_type, max_size>::const_iterator;
    using reverse_iterator = typename std::array<value_type, max_size>::reverse_iterator;
    using const_reverse_iterator = typename std::array<value_type, max_size>::const_reverse_iterator;

private:
    std::array<value_type, max_size> store_array_;  ///< 内部存储所用的数组
    [[no_unique_address]] KeyEqual key_equal_{};    ///< 用于比较键是否相等的函数

    /**
     * 断言不存在重复的键
     * @return 如果存在重复的键，返回 false
     */
    [[nodiscard]] constexpr bool assertNoneRepeatKeys_() const noexcept
    {
        std::array<key_type, max_size> keys;
        size_type key_count = 0;

        for (const auto& kv : store_array_) {
            for (size_type i = 0; i < key_count; ++i) {
                if (key_equal_(keys[i], kv.first)) {
                    return false;
                }
            }
            keys[key_count++] = kv.first;
        }

        return true;
    }

    /**
     * 查找特定的键
     * @tparam Iterator 迭代器的类型
     * @param key 待查找的键
     * @param begin_it 范围左区间
     * @param end_it 范围右区间
     * @return 查找到的键所在位置的迭代器。如果未查找到，返回 end_it
     */
    template<typename Iterator>
    constexpr Iterator findImpl_(const key_type& key, Iterator begin_it, Iterator end_it) const noexcept
    {
        for (auto it = begin_it; it != end_it; ++it) {
            if (key_equal_(it->first, key)) {
                return it;
            }
        }

        return end_it;
    }

public:
    consteval StaticMap() noexcept = default;
    consteval StaticMap(const StaticMap&) noexcept = default;
    consteval StaticMap(StaticMap&&) noexcept = default;

    ~StaticMap() noexcept = default;

    /**
     * 通过 std::array 类构造
     */
    explicit consteval StaticMap(const std::array<value_type, max_size>& init_arr) noexcept : store_array_(init_arr)
    {
        EMDEVIF_ASSERT(assertNoneRepeatKeys_(), "Have repeated keys!");
    }
    /**
     * @overload
     */
    explicit consteval StaticMap(const value_type (&init_arr)[max_size]) noexcept
        : store_array_(std::to_array(init_arr))
    {
        EMDEVIF_ASSERT(assertNoneRepeatKeys_(), "Have repeated keys!");
    }

    /**
     * 获取用于比较键是否相等的函数
     * @return 用于比较键是否相等的函数
     */
    constexpr KeyEqual key_eq() const noexcept
    {
        return key_equal_;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    /**
     * 获取最大存储的元素个数
     * @return 存储的元素个数
     */
    [[nodiscard]] constexpr size_type maxSize() const noexcept
    {
        return store_array_.size();
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return store_array_.size();
    }

    /**
     * 获取最大存储的大小（以字节为单位）
     * @return 存储的大小（以字节为单位）
     */
    [[nodiscard]] constexpr size_type maxSizeByte() const noexcept
    {
        return store_array_.size() * sizeof(mapped_type);
    }

    [[nodiscard]] constexpr size_type sizeByte() const noexcept
    {
        return store_array_.size() * sizeof(mapped_type);
    }

    /**
     * 获取底层存储所用数组的指针
     * @return 底层存储所用数组的指针
     */
    [[nodiscard]] constexpr value_type* data() noexcept
    {
        return store_array_.data();
    }
    /**
     * @overload
     */
    [[nodiscard]] constexpr const value_type* data() const noexcept
    {
        return store_array_.data();
    }

    [[nodiscard]] constexpr iterator begin() noexcept
    {
        return store_array_.begin();
    }
    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return store_array_.begin();
    }

    [[nodiscard]] constexpr iterator end() noexcept
    {
        return store_array_.end();
    }
    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return store_array_.end();
    }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept
    {
        return store_array_.cbegin();
    }
    [[nodiscard]] constexpr const_iterator cend() const noexcept
    {
        return store_array_.cend();
    }

    /**
     * 查找特定的键
     * @param key 待查找的键
     * @return 查找到的键所在位置的迭代器。如果待查找的键不存在，返回 end()。
     */
    constexpr const_iterator find(const KeyType& key) const noexcept
    {
        return findImpl_(key, cbegin(), cend());
    }

    /**
     * 获取给定键的映射值
     * @attention 无边界检查。如果键不存在，属于未定义行为。
     * @param key 待获取映射后的值的键
     * @return 给定键的对应值
     */
    [[nodiscard]] constexpr mapped_type& operator[](const key_type& key) noexcept
    {
        auto it = findImpl_(key, begin(), end());
        EMDEVIF_ASSERT(it != end(), "Out of range!");
        return it->second;
    }
    /**
     * @overload
     */
    [[nodiscard]] constexpr const mapped_type& operator[](const key_type& key) const noexcept
    {
        auto it = findImpl_(key, cbegin(), cend());
        EMDEVIF_ASSERT(it != cend(), "Out of range!");
        return it->second;
    }

    /**
     * 获取给定键的映射值的指针
     * @param key 待获取映射后的值的键
     * @return 给定键的对应值的指针。如果键不存在，返回 nullptr
     */
    [[nodiscard]] constexpr mapped_type* at(const key_type& key) noexcept
    {
        auto it = findImpl_(key, begin(), end());
        if (it == end()) {
            return nullptr;
        }

        return &it->second;
    }
    /**
     * @overload
     */
    [[nodiscard]] constexpr const mapped_type* at(const key_type& key) const noexcept
    {
        auto it = findImpl_(key, cbegin(), cend());
        if (it == cend()) {
            return nullptr;
        }

        return &it->second;
    }
};

/**
 * 构造 @ref emdevif::StaticMap 类
 * @tparam KeyType 键的类型
 * @tparam ValueType 值的类型
 * @tparam size 构造的 StaticMap 类的键值对数量
 * @param init_arr 初始化所用的 C 风格数组
 * @return 构造好的 StaticMap 类
 */
template<typename KeyType, typename ValueType, std::size_t size, typename KeyEqual = std::equal_to<KeyType>>
consteval auto makeStaticMap(const std::pair<KeyType, ValueType> (&init_arr)[size]) noexcept
{
    return StaticMap<KeyType, ValueType, size, KeyEqual>{init_arr};
}

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_DATA_CONTAINER_STATIC_MAP_HPP
