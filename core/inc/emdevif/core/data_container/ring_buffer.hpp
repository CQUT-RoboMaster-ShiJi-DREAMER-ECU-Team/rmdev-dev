/**
 * @file ring_buffer.hpp
 * @brief 环形缓冲区
 */

#pragma once
#ifndef EMDEVIF_CORE_DATA_CONTAINER_RING_BUFFER_HPP
#define EMDEVIF_CORE_DATA_CONTAINER_RING_BUFFER_HPP

#include <cstdint>
#include <cstring>

#include <array>
#include <concepts>
#include <span>
#include <type_traits>

namespace emdevif {

/**
 * 环形无符号整型类
 *
 * 可以用于环形缓冲区的指针的实现，当值达到最大值时会回绕到 0，值小于零之后会回到最大值减一
 * @tparam ValueType 储存值的类型
 * @tparam max_num 最大值
 */
template<std::unsigned_integral ValueType, ValueType max_num>
class RingUnsigned
{
public:
    constexpr RingUnsigned() noexcept : value_(0U) {}

    constexpr RingUnsigned(const RingUnsigned&) noexcept = default;
    constexpr RingUnsigned& operator=(const RingUnsigned&) noexcept = default;

    explicit constexpr RingUnsigned(const ValueType init_value) noexcept : value_(init_value % max_num) {}
    constexpr RingUnsigned& operator=(const ValueType init_value) noexcept
    {
        value_ = init_value % max_num;
        return *this;
    }

    constexpr ~RingUnsigned() noexcept = default;

public:
    /**
     * 自增数值，达到最大值后回绕归零
     */
    constexpr RingUnsigned operator++() noexcept
    {
        value_ = (value_ + 1U) % max_num;
        return *this;
    }
    /// @overload
    constexpr RingUnsigned operator++(int) noexcept
    {
        const auto temp = *this;
        ++(*this);
        return temp;
    }

    /**
     * 自减数值。如果数值已经是 0 之后再运行自减，会回绕到最大值减一
     */
    constexpr RingUnsigned operator--() noexcept
    {
        if (value_ == 0U) {
            value_ = max_num - 1;
        }
        else {
            --value_;
        }

        return *this;
    }
    /// @overload
    constexpr RingUnsigned operator--(int) noexcept
    {
        const auto temp = value_;
        --(*this);
        value_ = temp;
        return *this;
    }

    /**
     * 加法赋值运算符
     * @param rhs 要增加的值
     */
    constexpr RingUnsigned operator+=(const ValueType rhs) noexcept
    {
        value_ = (value_ + rhs) % max_num;
        return *this;
    }

    // ReSharper disable once CppDoxygenUnresolvedReference
    /**
     * 减法运算符。
     *
     * 不是直接做数值减法。通俗地讲, lhs - rhs 的表示的是 rhs 按照顺时针方向到 lhs 的距离，如下：
     * @verbatim
     * lhs > rhs 时：
     *
     *                        0   1
     *          (max_num - 1)       2
     *                   ...          3
     *                   ...       rhs ----+
     *                     lhs    ...      |
     *                      |              |
     *                      +--------------+
     *                              lhs - rhs 表示的是这一段距离
     *
     * lhs < rhs 时：
     *
     *     lhs - rhs 表示的是这一段距离
     *       +-----------------------------+
     *       |                             |
     *       |                0   1        |
     *       |  (max_num - 1)       2      |
     *       |           ...          3    |
     *       |           ...       lhs ----+
     *       +------------  rhs   ...
     * @endverbatim
     * @param lhs （隐藏的 this 形参），减号左侧的参数
     * @param rhs 减号右侧的参数
     * @return rhs 按照顺时针方向到 lhs 的距离
     */
    constexpr RingUnsigned operator-(const RingUnsigned rhs) const noexcept
    {
        const ValueType lhs_value = value_;
        const ValueType rhs_value = rhs.value_;

        if (lhs_value >= rhs_value) {
            return RingUnsigned(lhs_value - rhs_value);
        }
        else {
            return RingUnsigned(max_num - (rhs_value - lhs_value));
        }
    }

    constexpr operator ValueType() const noexcept  // NOLINT
    {
        return value_;
    }

private:
    std::size_t value_;  ///< 存储的值
};

/**
 * 环形缓冲区
 *
 * 此环形缓冲区能够适配以下两种场景：
 * 1. 复杂类型，固定大小。例如信号队列、消息队列（不同线程之间需要加互斥锁）。
 * 2. 基本类型，不定长度。例如串口接收。
 * @tparam Type 存储的类型
 * @tparam item_size 缓冲区大小
 */
template<typename Type, std::size_t item_size>
    requires(std::copyable<Type> && std::movable<Type>)
class RingBuffer
{
public:
    constexpr RingBuffer() noexcept = default;
    constexpr ~RingBuffer() noexcept = default;

public:
    /**
     * 清空缓冲区
     */
    constexpr void clear() noexcept
    {
        head_ = 0U;
        tail_ = 0U;
        count_ = 0U;
    }

    /**
     * 向缓冲区增加单个数据
     * @param item 待推送的值
     */
    constexpr void push(const Type& item) noexcept
    {
        if (isFull()) {
            buffer_[head_++] = item;
            ++tail_;
            return;
        }
        buffer_[head_++] = item;
        ++count_;
    }
    /// @overload
    constexpr void push(Type&& item) noexcept
    {
        if (isFull()) {
            buffer_[head_++] = std::move(item);
            ++tail_;
            return;
        }
        buffer_[head_++] = std::move(item);
        ++count_;
    }

    /**
     * 向缓冲区增加多个数据
     *
     * @note 如果 Type 满足 is_trivially_copyable，那么会使用 std::memcpy 实现。
     * @param items 待推送的值
     */
    constexpr void push(const std::span<const Type> items) noexcept
    {
        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<Type>) {
            for (const auto& item : items) {
                push(item);
            }
        }
        else {
            if (items.size() > remainSlots()) {
                for (const auto& item : items) {
                    push(item);
                }
                return;
            }
            if (item_size - head_ >= items.size()) {
                std::memcpy(&buffer_[head_], items.data(), sizeof(Type) * items.size());
                head_ += items.size();
            }
            else {
                const auto last_remain_slots = item_size - head_;

                std::memcpy(&buffer_[head_], items.data(), sizeof(Type) * last_remain_slots);
                std::memcpy(&buffer_[0], &items[last_remain_slots], sizeof(Type) * (items.size() - last_remain_slots));
                head_ += items.size();
            }
            count_ += items.size();
        }
    }
    /// @overload
    /// 向缓冲区增加多个数据，以移动的方式
    /// @note 由于存在移动语义，因此无法使用 std::memcpy 实现。
    constexpr void push(std::span<Type>&& items) noexcept
    {
        for (auto& item : items) {
            push(std::move(item));
        }
    }

    /**
     * 从缓冲区中移出末尾的数据
     * @return 末尾的数据
     */
    constexpr Type pop() noexcept
    {
        --count_;
        return buffer_[tail_++];
    }

    /**
     * 查看缓冲区中末尾的数据（但不会移除它）
     * @return 末尾的数据
     */
    constexpr Type peek() const noexcept
    {
        return buffer_[tail_];
    }

    /**
     * 从缓冲区末尾移除多个数据，并以拷贝的方式从形参返回被移除的数据
     *
     * @note 如果 Type 满足 is_trivially_copyable，那么会使用 std::memcpy 实现。
     * @param[in, out] items 返回被移除的数据，移除数据的数量与传入的 std::span 存储的大小相同。
     */
    constexpr void pop(std::span<Type> items) noexcept
    {
        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<Type>) {
            for (auto& item : items) {
                item = pop();
            }
        }
        else {
            if (items.size() > count_) {
                for (auto& item : items) {
                    item = pop();
                }
                return;
            }
            if (item_size - tail_ >= items.size()) {
                std::memcpy(items.data(), &buffer_[tail_], sizeof(Type) * items.size());
                tail_ += items.size();
            }
            else {
                const auto last_remain_slots = item_size - tail_;
                std::memcpy(items.data(), &buffer_[tail_], sizeof(Type) * last_remain_slots);
                std::memcpy(&items[last_remain_slots], &buffer_[0], sizeof(Type) * (items.size() - last_remain_slots));
                tail_ += items.size();
            }
            count_ -= items.size();
        }
    }

    /**
     * 从缓冲区末尾查看多个数据（不会移除数据，以拷贝的方式从形参返回要查看的数据）
     *
     * @note 如果 Type 满足 is_trivially_copyable，那么会使用 std::memcpy 实现。
     * @param[in, out] items 返回要查看的数据，查看的数量与传入的 std::span 存储的大小相同。
     */
    constexpr void peek(std::span<Type> items) const noexcept
    {
        if (std::is_constant_evaluated()) {
            for (auto& item : items) {
                item = peek();
            }
        }
        else {
            if (item_size - tail_ >= items.size()) {
                std::memcpy(items.data(), &buffer_[tail_], sizeof(Type) * items.size());
            }
            else {
                const auto last_remain_slots = item_size - tail_;
                std::memcpy(items.data(), &buffer_[tail_], sizeof(Type) * last_remain_slots);
                std::memcpy(&items[last_remain_slots], &buffer_[0], sizeof(Type) * (items.size() - last_remain_slots));
            }
        }
    }

    /**
     * 从缓冲区末尾查看数据的引用
     * @return 处于末尾的数据的引用
     */
    constexpr Type& peekRef() noexcept
    {
        return buffer_[tail_];
    }
    /// @overload
    constexpr const Type& peekRef() const noexcept
    {
        return buffer_[tail_];
    }

    /**
     * 丢弃数据
     *
     * 可以在通过 @ref peekRef 获取处于末尾的数据的引用，并处理了数据后，调用这个函数丢弃数据，
     * 从而避免了通过拷贝获取数据
     * @param slots 要丢弃的数据数量
     */
    constexpr void discard(std::size_t slots) noexcept
    {
        if (slots > count_) {
            slots = count_;
        }
        tail_ += slots;
        count_ -= slots;
    }

    /**
     * 获得已使用的数据数量
     * @return 已使用的数据数量
     */
    [[nodiscard]] constexpr std::size_t usedSlots() const noexcept
    {
        return count_;
    }

    /**
     * 获得已使用的数据数量大小（以字节为单位）
     * @return 已使用的数据数量字节大小
     */
    [[nodiscard]] constexpr std::size_t usedSizeByte() const noexcept
    {
        return sizeof(Type) * usedSlots();
    }

    /**
     * 判断缓冲区是否是空的
     * @return 缓冲区是否是空的
     */
    [[nodiscard]] constexpr bool isEmpty() const noexcept
    {
        return count_ == 0;
    }

    /**
     * 判断缓冲区是否是满的
     * @return 缓冲区是否是满的
     */
    [[nodiscard]] constexpr bool isFull() const noexcept
    {
        return count_ == item_size;
    }

    /**
     * 获取缓冲区剩余的数据数量
     * @return 剩余的数据数量
     */
    [[nodiscard]] constexpr std::size_t remainSlots() const noexcept
    {
        return item_size - count_;
    }

    /**
     * 获取缓冲区剩余的数据大小（以字节为单位）
     * @return 剩余的数据数量字节大小
     */
    [[nodiscard]] constexpr std::size_t remainSizeByte() const noexcept
    {
        return sizeof(Type) * remainSlots();
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    /**
     * 获取缓冲区能够容纳的数据数量
     * @return 缓冲区大小
     */
    [[nodiscard]] constexpr std::size_t size() const noexcept
    {
        return item_size;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    /**
     * 获取缓冲区能够容纳的大小（以字节为单位）
     * @return 缓冲区字节大小
     */
    [[nodiscard]] constexpr std::size_t sizeByte() const noexcept
    {
        return item_size * sizeof(Type);
    }

    /**
     * 获取下一个槽位的引用
     * @return 下一个缓冲区槽位的引用
     */
    constexpr Type& nextSlot() noexcept
    {
        return buffer_[head_];
    }
    /// @overload
    constexpr const Type& nextSlot() const noexcept
    {
        return buffer_[head_];
    }

    /**
     * 使用下一个缓冲区槽位，即获取了下一个槽位的引用后，占用它
     * @return 下一个缓冲区槽位的引用
     */
    constexpr Type& useNextSlot() noexcept
    {
        if (isFull()) {
            ++tail_;
            return buffer_[head_++];
        }
        ++count_;
        return buffer_[head_++];
    }
    /// @overload
    constexpr const Type& useNextSlot() const noexcept
    {
        return buffer_[head_++];
    }

    /**
     * 已经向缓冲区中增加了一定数据
     *
     * 可以在通过 @ref useNextSlot 获取处于下一个位置的引用，并通过引用增加了一些数据后，
     * 调用这个函数向缓冲区标记增加的数据的数量，从而避免了通过拷贝增加数据。
     * @param slots 已经向缓冲区推送了的槽位数
     */
    constexpr void havePushed(std::size_t slots) noexcept
    {
        if (slots > remainSlots()) {
            slots = remainSlots();
        }

        head_ += slots;
        count_ += slots;
    }

private:
    Type buffer_[item_size];                         ///< 缓冲区存储所用的数组
    RingUnsigned<std::size_t, item_size> head_{0U};  ///< 缓冲区头指针
    RingUnsigned<std::size_t, item_size> tail_{0U};  ///< 缓冲区尾指针
    std::size_t count_{0};                           ///< 已使用槽位数
};

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_DATA_CONTAINER_RING_BUFFER_HPP
