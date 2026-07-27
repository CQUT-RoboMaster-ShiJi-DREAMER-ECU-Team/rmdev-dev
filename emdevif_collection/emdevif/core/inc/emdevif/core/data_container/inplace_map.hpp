/**
 * @file inplace_map.hpp
 * @brief 固定容量的原地有序映射表
 *
 * 基于两个 InplaceDynamicArray 分别存储 key 与 value 的扁平有序 map。
 * 所有可能超出容量的操作均返回错误码，不抛异常。
 */

#pragma once
#ifndef EMDEVIF_CORE_DATA_CONTAINER_INPLACE_MAP_HPP
#define EMDEVIF_CORE_DATA_CONTAINER_INPLACE_MAP_HPP

#include <cstddef>

#include <algorithm>
#include <compare>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>

#include "emdevif/core/data_container/inplace_dynamic_array.hpp"
#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"

namespace emdevif {

// 前向声明 ---------------------------------------------------------------

template<typename Key, typename T, std::size_t N, typename Compare, typename KeyContainer, typename MappedContainer>
class InplaceMap;

namespace detail {

// Arrow proxy ----------------------------------------------------------------

template<typename KeyRef, typename MappedRef>
struct InplaceMapArrowProxy {
    std::pair<KeyRef, MappedRef> value;

    constexpr InplaceMapArrowProxy(KeyRef key, MappedRef mapped) : value(key, mapped) {}

    [[nodiscard]] constexpr const std::pair<KeyRef, MappedRef>* operator->() const noexcept
    {
        return &value;
    }

    [[nodiscard]] constexpr std::pair<KeyRef, MappedRef>* operator->() noexcept
    {
        return &value;
    }
};

// 迭代器 ---------------------------------------------------------------------

template<typename Key,
         typename T,
         std::size_t N,
         typename Compare,
         typename KeyContainer,
         typename MappedContainer,
         bool IsConst>
class InplaceMapIterator
{
    friend class emdevif::InplaceMap<Key, T, N, Compare, KeyContainer, MappedContainer>;

    using map_ptr = std::conditional_t<IsConst,
                                       const emdevif::InplaceMap<Key, T, N, Compare, KeyContainer, MappedContainer>*,
                                       emdevif::InplaceMap<Key, T, N, Compare, KeyContainer, MappedContainer>*>;

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::pair<const Key, T>;
    using difference_type = std::ptrdiff_t;
    using reference = std::pair<const Key&, std::conditional_t<IsConst, const T&, T&>>;
    using pointer = InplaceMapArrowProxy<const Key&, std::conditional_t<IsConst, const T&, T&>>;
    using ptr_type = map_ptr;

private:
    map_ptr map_;
    std::size_t idx_;

    constexpr InplaceMapIterator(map_ptr m, std::size_t i) noexcept : map_(m), idx_(i) {}

public:
    constexpr InplaceMapIterator() noexcept : map_(nullptr), idx_(0) {}

    template<bool OtherConst>
        requires(IsConst && !OtherConst)
    constexpr InplaceMapIterator(
        const InplaceMapIterator<Key, T, N, Compare, KeyContainer, MappedContainer, OtherConst>& other) noexcept
        : map_(other.ptr()), idx_(other.index())
    {
    }

    [[nodiscard]] constexpr reference operator*() const
    {
        return reference(map_->keys_[idx_], map_->values_[idx_]);
    }

    [[nodiscard]] constexpr pointer operator->() const
    {
        return pointer(map_->keys_[idx_], map_->values_[idx_]);
    }

    [[nodiscard]] constexpr reference operator[](difference_type n) const
    {
        const auto i = static_cast<std::size_t>(static_cast<difference_type>(idx_) + n);
        return reference(map_->keys_[i], map_->values_[i]);
    }

    constexpr InplaceMapIterator& operator++() noexcept
    {
        ++idx_;
        return *this;
    }

    constexpr InplaceMapIterator operator++(int) noexcept
    {
        auto temp = *this;
        ++idx_;
        return temp;
    }

    constexpr InplaceMapIterator& operator--() noexcept
    {
        --idx_;
        return *this;
    }

    constexpr InplaceMapIterator operator--(int) noexcept
    {
        auto temp = *this;
        --idx_;
        return temp;
    }

    constexpr InplaceMapIterator& operator+=(difference_type n) noexcept
    {
        idx_ = static_cast<std::size_t>(static_cast<difference_type>(idx_) + n);
        return *this;
    }

    constexpr InplaceMapIterator& operator-=(difference_type n) noexcept
    {
        idx_ = static_cast<std::size_t>(static_cast<difference_type>(idx_) - n);
        return *this;
    }

    [[nodiscard]] constexpr InplaceMapIterator operator+(difference_type n) const noexcept
    {
        return InplaceMapIterator(map_, static_cast<std::size_t>(static_cast<difference_type>(idx_) + n));
    }

    [[nodiscard]] constexpr InplaceMapIterator operator-(difference_type n) const noexcept
    {
        return InplaceMapIterator(map_, static_cast<std::size_t>(static_cast<difference_type>(idx_) - n));
    }

    [[nodiscard]] constexpr difference_type operator-(const InplaceMapIterator& other) const noexcept
    {
        return static_cast<difference_type>(idx_) - static_cast<difference_type>(other.idx_);
    }

    [[nodiscard]] constexpr bool operator==(const InplaceMapIterator& other) const noexcept
    {
        return idx_ == other.idx_;
    }

    [[nodiscard]] constexpr auto operator<=>(const InplaceMapIterator& other) const noexcept
    {
        return idx_ <=> other.idx_;
    }

    [[nodiscard]] constexpr ptr_type ptr() const noexcept
    {
        return map_;
    }

    [[nodiscard]] constexpr std::size_t index() const noexcept
    {
        return idx_;
    }

    [[nodiscard]] friend constexpr InplaceMapIterator operator+(difference_type n,
                                                                const InplaceMapIterator& it) noexcept
    {
        return it + n;
    }
};

// 反向迭代器 ------------------------------------------------------------------

template<typename Iter>
class InplaceMapReverseIterator : public std::reverse_iterator<Iter>
{
    using base_type = std::reverse_iterator<Iter>;

public:
    using pointer = typename base_type::pointer;

    InplaceMapReverseIterator() noexcept : base_type() {}

    explicit constexpr InplaceMapReverseIterator(Iter x) noexcept : base_type(x) {}

    [[nodiscard]] constexpr pointer operator->() const
    {
        Iter tmp = base_type::base();
        --tmp;
        return tmp.operator->();
    }
};

}  // namespace detail

// InplaceMap -----------------------------------------------------------------

template<typename Key,
         typename T,
         std::size_t N,
         typename Compare = std::less<Key>,
         typename KeyContainer = InplaceDynamicArray<Key, N>,
         typename MappedContainer = InplaceDynamicArray<T, N>>
class InplaceMap
{
public:
    static_assert(N > 0, "InplaceMap capacity must be greater than zero");

    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const key_type, mapped_type>;
    using key_compare = Compare;
    using reference = std::pair<const key_type&, mapped_type&>;
    using const_reference = std::pair<const key_type&, const mapped_type&>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = detail::InplaceMapIterator<Key, T, N, Compare, KeyContainer, MappedContainer, false>;
    using const_iterator = detail::InplaceMapIterator<Key, T, N, Compare, KeyContainer, MappedContainer, true>;
    using reverse_iterator = detail::InplaceMapReverseIterator<iterator>;
    using const_reverse_iterator = detail::InplaceMapReverseIterator<const_iterator>;
    using key_container_type = KeyContainer;
    using mapped_container_type = MappedContainer;

    /**
     * 值比较器
     */
    struct value_compare : private key_compare {
        friend class InplaceMap;

        value_compare(key_compare c) : key_compare(c) {}

    public:
        [[nodiscard]] constexpr bool operator()(const value_type& a, const value_type& b) const
        {
            const key_compare& comp = *this;
            return comp(a.first, b.first);
        }
    };

private:
    key_container_type keys_;
    mapped_container_type values_;
    key_compare comp_;

    friend class detail::InplaceMapIterator<Key, T, N, Compare, KeyContainer, MappedContainer, false>;
    friend class detail::InplaceMapIterator<Key, T, N, Compare, KeyContainer, MappedContainer, true>;

    [[nodiscard]] constexpr const_iterator lowerBoundImpl_(const key_type& key) const
    {
        return const_iterator(
            this,
            static_cast<size_type>(
                std::distance(keys_.cbegin(), std::lower_bound(keys_.cbegin(), keys_.cend(), key, comp_))));
    }

    [[nodiscard]] constexpr iterator lowerBoundImpl_(const key_type& key)
    {
        return iterator(this,
                        static_cast<size_type>(
                            std::distance(keys_.cbegin(), std::lower_bound(keys_.cbegin(), keys_.cend(), key, comp_))));
    }

    [[nodiscard]] constexpr const_iterator findImpl_(const key_type& key) const
    {
        auto it = lowerBoundImpl_(key);
        if (it != cend() && !comp_(key, keys_[it.index()])) {
            return it;
        }
        return cend();
    }

    [[nodiscard]] constexpr iterator findImpl_(const key_type& key)
    {
        auto it = lowerBoundImpl_(key);
        if (it != end() && !comp_(key, keys_[it.index()])) {
            return it;
        }
        return end();
    }

public:
    // ---------- 容量 ----------

    [[nodiscard]] static constexpr size_type capacity() noexcept
    {
        return N;
    }

    [[nodiscard]] static constexpr size_type maxSize() noexcept
    {
        return N;
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return keys_.size();
    }

    [[nodiscard]] constexpr bool isEmpty() const noexcept
    {
        return keys_.isEmpty();
    }

    [[nodiscard]] constexpr bool isFull() const noexcept
    {
        return keys_.isFull();
    }

    // ---------- 构造 / 析构 ----------

    constexpr InplaceMap() : comp_() {}

    explicit constexpr InplaceMap(const key_compare& comp) : comp_(comp) {}

    template<typename InputIt>
    explicit constexpr InplaceMap(InputIt first, InputIt last) : comp_()
    {
        std::ranges::for_each(std::ranges::subrange(first, last),
                              [this](const value_type& v) { insert(v); });
    }

    template<typename InputIt>
    explicit constexpr InplaceMap(InputIt first, InputIt last, const key_compare& comp) : comp_(comp)
    {
        std::ranges::for_each(std::ranges::subrange(first, last),
                              [this](const value_type& v) { insert(v); });
    }

    constexpr InplaceMap(std::initializer_list<value_type> ilist) : comp_()
    {
        EMDEVIF_ASSERT(ilist.size() <= N, "[InplaceMap]: initializer list too large");
        insert(ilist);
    }

    constexpr InplaceMap(std::initializer_list<value_type> ilist, const key_compare& comp) : comp_(comp)
    {
        EMDEVIF_ASSERT(ilist.size() <= N, "[InplaceMap]: initializer list too large");
        insert(ilist);
    }

    constexpr InplaceMap(const InplaceMap& other) = default;

    constexpr InplaceMap(InplaceMap&& other) noexcept(std::is_nothrow_move_constructible_v<key_container_type> &&
                                                      std::is_nothrow_move_constructible_v<mapped_container_type>)
        : keys_(std::move(other.keys_)), values_(std::move(other.values_)), comp_(std::move(other.comp_))
    {
    }

    constexpr ~InplaceMap() = default;

    // ---------- 赋值 ----------

    constexpr InplaceMap& operator=(const InplaceMap& other) = default;

    constexpr InplaceMap& operator=(InplaceMap&& other) noexcept(
        std::is_nothrow_move_assignable_v<key_container_type> &&
        std::is_nothrow_move_assignable_v<mapped_container_type>)
    {
        if (this == &other) {
            return *this;
        }
        keys_ = std::move(other.keys_);
        values_ = std::move(other.values_);
        comp_ = std::move(other.comp_);
        return *this;
    }

    constexpr InplaceMap& operator=(std::initializer_list<value_type> ilist)
    {
        EMDEVIF_ASSERT(ilist.size() <= N, "[InplaceMap]: initializer list too large");
        clear();
        insert(ilist);
        return *this;
    }

    // ---------- 元素访问 ----------

    /**
     * 查找并返回 key 对应值的指针
     * @param key 待查找的键
     * @return 值指针，未找到返回 nullptr
     */
    [[nodiscard]] constexpr mapped_type* at(const key_type& key)
    {
        auto it = findImpl_(key);
        if (it == end()) {
            return nullptr;
        }
        return &values_[it.index()];
    }

    /**
     * @overload
     */
    [[nodiscard]] constexpr const mapped_type* at(const key_type& key) const
    {
        auto it = findImpl_(key);
        if (it == cend()) {
            return nullptr;
        }
        return &values_[it.index()];
    }

    /**
     * 通过 key 访问值（不隐式插入，key 必须存在）
     * @param key 待访问的键
     * @return 值引用
     */
    [[nodiscard]] constexpr mapped_type& operator[](const key_type& key)
    {
        auto it = findImpl_(key);
        EMDEVIF_ASSERT(it != end(), "[InplaceMap]: key not found");
        return values_[it.index()];
    }

    /**
     * @overload
     */
    [[nodiscard]] constexpr const mapped_type& operator[](const key_type& key) const
    {
        auto it = findImpl_(key);
        EMDEVIF_ASSERT(it != cend(), "[InplaceMap]: key not found");
        return values_[it.index()];
    }

    /**
     * 获取或创建 key 对应的值
     * @param key 键
     * @return 值指针，容量不足时返回 nullptr
     */
    [[nodiscard]] constexpr mapped_type* getOrCreate(const key_type& key)
    {
        auto it = lowerBoundImpl_(key);
        const auto idx = it.index();
        if (idx < keys_.size() && !comp_(key, keys_[idx])) {
            return &values_[idx];
        }
        if (keys_.isFull()) {
            return nullptr;
        }
        if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(idx), key).second != ErrorCode::Success) {
            return nullptr;
        }
        if (values_.insert(values_.cbegin() + static_cast<difference_type>(idx), mapped_type{}).second !=
            ErrorCode::Success) {
            // 理论上不会失败，因为 keys_ 与 values_ 容量相同
            keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
            return nullptr;
        }
        return &values_[idx];
    }

    /**
     * @overload
     */
    [[nodiscard]] constexpr mapped_type* getOrCreate(key_type&& key)
    {
        auto it = lowerBoundImpl_(key);
        const auto idx = it.index();
        if (idx < keys_.size() && !comp_(key, keys_[idx])) {
            return &values_[idx];
        }
        if (keys_.isFull()) {
            return nullptr;
        }
        if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(idx), std::move(key)).second !=
            ErrorCode::Success) {
            return nullptr;
        }
        if (values_.insert(values_.cbegin() + static_cast<difference_type>(idx), mapped_type{}).second !=
            ErrorCode::Success) {
            keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
            return nullptr;
        }
        return &values_[idx];
    }

    // ---------- 迭代器 ----------

    [[nodiscard]] constexpr iterator begin() noexcept
    {
        return iterator(this, 0);
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return const_iterator(this, 0);
    }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator(this, 0);
    }

    [[nodiscard]] constexpr iterator end() noexcept
    {
        return iterator(this, keys_.size());
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return const_iterator(this, keys_.size());
    }

    [[nodiscard]] constexpr const_iterator cend() const noexcept
    {
        return const_iterator(this, keys_.size());
    }

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }

    [[nodiscard]] constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    // ---------- 修改 ----------

    constexpr void clear() noexcept
    {
        keys_.clear();
        values_.clear();
    }

    /**
     * 插入键值对
     * @param value 待插入键值对
     * @return first 为迭代器，second 为错误码
     */
    constexpr std::pair<iterator, ErrorCode> insert(const value_type& value)
    {
        return insertImpl_(value.first, value.second);
    }

    /**
     * @overload
     */
    constexpr std::pair<iterator, ErrorCode> insert(value_type&& value)
    {
        return insertImpl_(std::move(value.first), std::move(value.second));
    }

    /**
     * 在 hint 附近插入键值对
     * @param hint 提示位置
     * @param value 待插入键值对
     * @return first 为迭代器，second 为错误码
     */
    constexpr std::pair<iterator, ErrorCode> insert(const_iterator hint, const value_type& value)
    {
        return insertHintImpl_(hint, value.first, value.second);
    }

    /**
     * @overload
     */
    constexpr std::pair<iterator, ErrorCode> insert(const_iterator hint, value_type&& value)
    {
        return insertHintImpl_(hint, std::move(value.first), std::move(value.second));
    }

    /**
     * 插入迭代器范围
     * @tparam InputIt 输入迭代器类型
     * @param first 范围起始
     * @param last 范围结束
     * @retval ErrorCode::Success 全部插入成功或遇到已存在键
     * @retval ErrorCode::Full 容量不足，可能已插入部分元素
     */
    template<typename InputIt>
    constexpr ErrorCode insert(InputIt first, InputIt last)
    {
        return std::ranges::any_of(std::ranges::subrange(first, last),
                                   [this](const value_type& v) {
                                       return insert(v).second == ErrorCode::Full;
                                   })
                   ? ErrorCode::Full
                   : ErrorCode::Success;
    }

    /**
     * 插入初始化列表
     * @param ilist 初始化列表
     * @retval ErrorCode::Success 全部插入成功或遇到已存在键
     * @retval ErrorCode::Full 容量不足，可能已插入部分元素
     */
    constexpr ErrorCode insert(std::initializer_list<value_type> ilist)
    {
        return insert(ilist.begin(), ilist.end());
    }

    /**
     * 插入或赋值
     * @param key 键
     * @param value 值
     * @return first 为迭代器，second 为错误码
     */
    constexpr std::pair<iterator, ErrorCode> insertOrAssign(const key_type& key, mapped_type&& value)
    {
        auto it = lowerBoundImpl_(key);
        const auto idx = it.index();
        if (idx < keys_.size() && !comp_(key, keys_[idx])) {
            values_[idx] = std::move(value);
            return {iterator(this, idx), ErrorCode::AlreadyExists};
        }
        if (keys_.isFull()) {
            return {end(), ErrorCode::Full};
        }
        if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(idx), key).second != ErrorCode::Success) {
            return {end(), ErrorCode::Full};
        }
        if (values_.insert(values_.cbegin() + static_cast<difference_type>(idx), std::move(value)).second !=
            ErrorCode::Success) {
            keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
            return {end(), ErrorCode::Full};
        }
        return {iterator(this, idx), ErrorCode::Success};
    }

    /**
     * @overload
     */
    constexpr std::pair<iterator, ErrorCode> insertOrAssign(key_type&& key, mapped_type&& value)
    {
        auto it = lowerBoundImpl_(key);
        const auto idx = it.index();
        if (idx < keys_.size() && !comp_(key, keys_[idx])) {
            values_[idx] = std::move(value);
            return {iterator(this, idx), ErrorCode::AlreadyExists};
        }
        if (keys_.isFull()) {
            return {end(), ErrorCode::Full};
        }
        if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(idx), std::move(key)).second !=
            ErrorCode::Success) {
            return {end(), ErrorCode::Full};
        }
        if (values_.insert(values_.cbegin() + static_cast<difference_type>(idx), std::move(value)).second !=
            ErrorCode::Success) {
            keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
            return {end(), ErrorCode::Full};
        }
        return {iterator(this, idx), ErrorCode::Success};
    }

    /**
     * 原位构造插入
     * @tparam Args 值构造参数类型
     * @param key 键
     * @param args 值构造参数
     * @return first 为迭代器，second 为错误码
     */
    template<typename... Args>
    constexpr std::pair<iterator, ErrorCode> emplace(key_type&& key, Args&&... args)
    {
        auto it = lowerBoundImpl_(key);
        const auto idx = it.index();
        if (idx < keys_.size() && !comp_(key, keys_[idx])) {
            return {iterator(this, idx), ErrorCode::AlreadyExists};
        }
        if (keys_.isFull()) {
            return {end(), ErrorCode::Full};
        }
        if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(idx), std::move(key)).second !=
            ErrorCode::Success) {
            return {end(), ErrorCode::Full};
        }
        if (values_.emplace(values_.cbegin() + static_cast<difference_type>(idx), std::forward<Args>(args)...).second !=
            ErrorCode::Success) {
            keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
            return {end(), ErrorCode::Full};
        }
        return {iterator(this, idx), ErrorCode::Success};
    }

    /**
     * 在 hint 附近原位构造插入
     * @tparam Args 值构造参数类型
     * @param hint 提示位置
     * @param key 键
     * @param args 值构造参数
     * @return first 为迭代器，second 为错误码
     */
    template<typename... Args>
    constexpr std::pair<iterator, ErrorCode> emplaceHint(const_iterator hint, key_type&& key, Args&&... args)
    {
        if (hint != cend()) {
            const auto h_idx = hint.index();
            if ((h_idx == 0 || comp_(keys_[h_idx - 1], key)) && (h_idx == keys_.size() || !comp_(key, keys_[h_idx]))) {
                if (h_idx < keys_.size() && !comp_(key, keys_[h_idx]) && !comp_(keys_[h_idx], key)) {
                    return {iterator(this, h_idx), ErrorCode::AlreadyExists};
                }
                if (!keys_.isFull()) {
                    if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(h_idx), std::move(key)).second !=
                        ErrorCode::Success) {
                        return {end(), ErrorCode::Full};
                    }
                    if (values_
                            .emplace(values_.cbegin() + static_cast<difference_type>(h_idx),
                                     std::forward<Args>(args)...)
                            .second != ErrorCode::Success) {
                        keys_.erase(keys_.cbegin() + static_cast<difference_type>(h_idx));
                        return {end(), ErrorCode::Full};
                    }
                    return {iterator(this, h_idx), ErrorCode::Success};
                }
                return {end(), ErrorCode::Full};
            }
        }
        return emplace(std::move(key), std::forward<Args>(args)...);
    }

    /**
     * 尝试原位构造插入（不替换已有键）
     * @tparam Args 值构造参数类型
     * @param key 键
     * @param args 值构造参数
     * @return first 为迭代器，second 为是否插入成功
     */
    template<typename... Args>
    [[nodiscard]] constexpr std::pair<iterator, bool> tryEmplace(const key_type& key, Args&&... args)
    {
        auto it = lowerBoundImpl_(key);
        const auto idx = it.index();
        if (idx < keys_.size() && !comp_(key, keys_[idx])) {
            return {iterator(this, idx), false};
        }
        if (keys_.isFull()) {
            return {end(), false};
        }
        if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(idx), key).second != ErrorCode::Success) {
            return {end(), false};
        }
        if (values_.emplace(values_.cbegin() + static_cast<difference_type>(idx), std::forward<Args>(args)...).second !=
            ErrorCode::Success) {
            keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
            return {end(), false};
        }
        return {iterator(this, idx), true};
    }

    /**
     * @overload
     */
    template<typename... Args>
    [[nodiscard]] constexpr std::pair<iterator, bool> tryEmplace(key_type&& key, Args&&... args)
    {
        auto it = lowerBoundImpl_(key);
        const auto idx = it.index();
        if (idx < keys_.size() && !comp_(key, keys_[idx])) {
            return {iterator(this, idx), false};
        }
        if (keys_.isFull()) {
            return {end(), false};
        }
        if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(idx), std::move(key)).second !=
            ErrorCode::Success) {
            return {end(), false};
        }
        if (values_.emplace(values_.cbegin() + static_cast<difference_type>(idx), std::forward<Args>(args)...).second !=
            ErrorCode::Success) {
            keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
            return {end(), false};
        }
        return {iterator(this, idx), true};
    }

    /**
     * 在 hint 附近尝试原位构造插入
     * @tparam Args 值构造参数类型
     * @param hint 提示位置
     * @param key 键
     * @param args 值构造参数
     * @return 插入位置迭代器，失败返回 end()
     */
    template<typename... Args>
    [[nodiscard]] constexpr iterator tryEmplaceHint(const_iterator hint, const key_type& key, Args&&... args)
    {
        if (hint != cend()) {
            const auto h_idx = hint.index();
            if ((h_idx == 0 || comp_(keys_[h_idx - 1], key)) && (h_idx == keys_.size() || !comp_(key, keys_[h_idx]))) {
                if (h_idx < keys_.size() && !comp_(key, keys_[h_idx]) && !comp_(keys_[h_idx], key)) {
                    return iterator(this, h_idx);
                }
                if (!keys_.isFull()) {
                    if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(h_idx), key).second !=
                        ErrorCode::Success) {
                        return end();
                    }
                    if (values_
                            .emplace(values_.cbegin() + static_cast<difference_type>(h_idx),
                                     std::forward<Args>(args)...)
                            .second != ErrorCode::Success) {
                        keys_.erase(keys_.cbegin() + static_cast<difference_type>(h_idx));
                        return end();
                    }
                    return iterator(this, h_idx);
                }
                return end();
            }
        }
        return tryEmplace(key, std::forward<Args>(args)...).first;
    }

    /**
     * @overload
     */
    template<typename... Args>
    [[nodiscard]] constexpr iterator tryEmplaceHint(const_iterator hint, key_type&& key, Args&&... args)
    {
        if (hint != cend()) {
            const auto h_idx = hint.index();
            if ((h_idx == 0 || comp_(keys_[h_idx - 1], key)) && (h_idx == keys_.size() || !comp_(key, keys_[h_idx]))) {
                if (h_idx < keys_.size() && !comp_(key, keys_[h_idx]) && !comp_(keys_[h_idx], key)) {
                    return iterator(this, h_idx);
                }
                if (!keys_.isFull()) {
                    if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(h_idx), std::move(key)).second !=
                        ErrorCode::Success) {
                        return end();
                    }
                    if (values_
                            .emplace(values_.cbegin() + static_cast<difference_type>(h_idx),
                                     std::forward<Args>(args)...)
                            .second != ErrorCode::Success) {
                        keys_.erase(keys_.cbegin() + static_cast<difference_type>(h_idx));
                        return end();
                    }
                    return iterator(this, h_idx);
                }
                return end();
            }
        }
        return tryEmplace(std::move(key), std::forward<Args>(args)...).first;
    }

    /**
     * 删除指定键
     * @param key 待删除键
     * @return 删除的元素数量（0 或 1）
     */
    constexpr size_type erase(const key_type& key)
    {
        auto it = findImpl_(key);
        if (it == end()) {
            return 0;
        }
        const auto idx = it.index();
        keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
        values_.erase(values_.cbegin() + static_cast<difference_type>(idx));
        return 1;
    }

    /**
     * 删除指定位置元素
     * @param pos 待删除位置
     * @return 删除后下一个位置迭代器
     */
    constexpr iterator erase(const_iterator pos)
    {
        const auto idx = pos.index();
        keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
        values_.erase(values_.cbegin() + static_cast<difference_type>(idx));
        return iterator(this, idx);
    }

    /**
     * 删除范围 [first, last) 内元素
     * @param first 范围起始
     * @param last 范围结束
     * @return 删除后下一个位置迭代器
     */
    constexpr iterator erase(const_iterator first, const_iterator last)
    {
        const auto f_idx = first.index();
        const auto l_idx = last.index();
        if (f_idx == l_idx) {
            return iterator(this, f_idx);
        }
        keys_.erase(keys_.cbegin() + static_cast<difference_type>(f_idx),
                    keys_.cbegin() + static_cast<difference_type>(l_idx));
        values_.erase(values_.cbegin() + static_cast<difference_type>(f_idx),
                      values_.cbegin() + static_cast<difference_type>(l_idx));
        return iterator(this, f_idx);
    }

    /**
     * 交换两个 map
     * @param other 另一个 map
     */
    constexpr void swap(InplaceMap& other) noexcept(std::is_nothrow_swappable_v<key_container_type> &&
                                                    std::is_nothrow_swappable_v<mapped_container_type>)
    {
        using std::swap;
        keys_.swap(other.keys_);
        values_.swap(other.values_);
        swap(comp_, other.comp_);
    }

    // ---------- 查找 ----------

    [[nodiscard]] constexpr size_type count(const key_type& key) const
    {
        return contains(key) ? 1 : 0;
    }

    [[nodiscard]] constexpr bool contains(const key_type& key) const
    {
        return findImpl_(key) != cend();
    }

    [[nodiscard]] constexpr iterator find(const key_type& key)
    {
        return findImpl_(key);
    }

    [[nodiscard]] constexpr const_iterator find(const key_type& key) const
    {
        return findImpl_(key);
    }

    [[nodiscard]] constexpr iterator lowerBound(const key_type& key)
    {
        return lowerBoundImpl_(key);
    }

    [[nodiscard]] constexpr const_iterator lowerBound(const key_type& key) const
    {
        return lowerBoundImpl_(key);
    }

    [[nodiscard]] constexpr iterator upperBound(const key_type& key)
    {
        auto it = lowerBoundImpl_(key);
        if (it != end() && !comp_(key, keys_[it.index()])) {
            ++it;
        }
        return it;
    }

    [[nodiscard]] constexpr const_iterator upperBound(const key_type& key) const
    {
        auto it = lowerBoundImpl_(key);
        if (it != cend() && !comp_(key, keys_[it.index()])) {
            ++it;
        }
        return it;
    }

    [[nodiscard]] constexpr std::pair<iterator, iterator> equalRange(const key_type& key)
    {
        auto lb = lowerBound(key);
        auto ub = lb;
        if (ub != end() && !comp_(key, keys_[ub.index()])) {
            ++ub;
        }
        return {lb, ub};
    }

    [[nodiscard]] constexpr std::pair<const_iterator, const_iterator> equalRange(const key_type& key) const
    {
        auto lb = lowerBound(key);
        auto ub = lb;
        if (ub != cend() && !comp_(key, keys_[ub.index()])) {
            ++ub;
        }
        return {lb, ub};
    }

    // ---------- 比较器 / 容器访问 ----------

    [[nodiscard]] constexpr key_compare keyComp() const
    {
        return comp_;
    }

    [[nodiscard]] constexpr value_compare valueComp() const
    {
        return value_compare(comp_);
    }

    [[nodiscard]] constexpr const key_container_type& keys() const noexcept
    {
        return keys_;
    }

    [[nodiscard]] constexpr const mapped_container_type& values() const noexcept
    {
        return values_;
    }

    // ---------- 比较运算符 ----------

    [[nodiscard]] friend constexpr auto operator<=>(const InplaceMap& a, const InplaceMap& b)
    {
        return std::lexicographical_compare_three_way(a.cbegin(), a.cend(), b.cbegin(), b.cend());
    }

    [[nodiscard]] friend constexpr bool operator==(const InplaceMap& a, const InplaceMap& b)
    {
        return a.size() == b.size() && std::equal(a.cbegin(), a.cend(), b.cbegin());
    }

    // ---------- 非成员 swap（hidden friend）----------

    /**
     * 交换两个 InplaceMap
     * @param a 左操作数
     * @param b 右操作数
     */
    friend constexpr void swap(InplaceMap& a, InplaceMap& b) noexcept(noexcept(a.swap(b)))
    {
        a.swap(b);
    }

private:
    template<typename K, typename V>
    constexpr std::pair<iterator, ErrorCode> insertImpl_(K&& key, V&& value)
    {
        auto it = lowerBoundImpl_(key);
        const auto idx = it.index();
        if (idx < keys_.size() && !comp_(key, keys_[idx])) {
            return {iterator(this, idx), ErrorCode::AlreadyExists};
        }
        if (keys_.isFull()) {
            return {end(), ErrorCode::Full};
        }
        if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(idx), std::forward<K>(key)).second !=
            ErrorCode::Success) {
            return {end(), ErrorCode::Full};
        }
        if (values_.insert(values_.cbegin() + static_cast<difference_type>(idx), std::forward<V>(value)).second !=
            ErrorCode::Success) {
            keys_.erase(keys_.cbegin() + static_cast<difference_type>(idx));
            return {end(), ErrorCode::Full};
        }
        return {iterator(this, idx), ErrorCode::Success};
    }

    template<typename K, typename V>
    constexpr std::pair<iterator, ErrorCode> insertHintImpl_(const_iterator hint, K&& key, V&& value)
    {
        if (hint != cend()) {
            const auto h_idx = hint.index();
            if ((h_idx == 0 || comp_(keys_[h_idx - 1], key)) && (h_idx == keys_.size() || !comp_(key, keys_[h_idx]))) {
                if (h_idx < keys_.size() && !comp_(key, keys_[h_idx]) && !comp_(keys_[h_idx], key)) {
                    return {iterator(this, h_idx), ErrorCode::AlreadyExists};
                }
                if (!keys_.isFull()) {
                    if (keys_.insert(keys_.cbegin() + static_cast<difference_type>(h_idx), std::forward<K>(key))
                            .second != ErrorCode::Success) {
                        return {end(), ErrorCode::Full};
                    }
                    if (values_.insert(values_.cbegin() + static_cast<difference_type>(h_idx), std::forward<V>(value))
                            .second != ErrorCode::Success) {
                        keys_.erase(keys_.cbegin() + static_cast<difference_type>(h_idx));
                        return {end(), ErrorCode::Full};
                    }
                    return {iterator(this, h_idx), ErrorCode::Success};
                }
                return {end(), ErrorCode::Full};
            }
        }
        return insertImpl_(std::forward<K>(key), std::forward<V>(value));
    }
};

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_DATA_CONTAINER_INPLACE_MAP_HPP
