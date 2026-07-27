/**
 * @file motor.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_DEVICE_MODEL_MOTOR_HPP
#define RMDEV_DEVICE_MODEL_MOTOR_HPP

#include <cstdint>

#include <array>
#include <compare>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>

#include <mp-units/systems/si.h>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/concepts.hpp"
#include "emdevif/core/error_handler.hpp"

namespace rmdev {

inline namespace units {
inline namespace motor_units {

/// 转每分（== 1/60 Hz）
constexpr struct rpm final : mp_units::named_unit<"rpm",
                                                  mp_units::mag_ratio<1, 60> * mp_units::si::unit_symbols::Hz,
                                                  mp_units::kind_of<mp_units::isq::frequency>> {
} rpm{};

}  // namespace motor_units
}  // namespace units

/**
 * @brief 电机多圈记录记录器
 * @tparam T 位置值的数据类型
 */
template<emdevif::ArithmeticType T>
class MotorRotateRecorder
{
public:
    using DataType = T;

private:
    DataType mid_pos_;          /**< 中间位置值 */
    DataType high_pos_;         /**< 高位置值 */
    DataType low_pos_;          /**< 低位置值 */
    int_fast32_t rotate_count_; /**< 旋转计数 */
    DataType now_pos_;          /**< 当前位置值 */
    DataType previous_pos_;     /**< 之前位置值 */

public:
    /**
     * @brief 构造多圈记录器
     * @param mid_pos 中间位置值
     * @param high_pos 高位置值
     * @param low_pos 低位置值
     * @param now_pos 当前位置值
     * @param previous_pos 之前位置值
     */
    constexpr MotorRotateRecorder(const DataType mid_pos,
                                  const DataType high_pos,
                                  const DataType low_pos,
                                  const DataType now_pos,
                                  const DataType previous_pos) noexcept
        : mid_pos_(mid_pos),
          high_pos_(high_pos),
          low_pos_(low_pos),
          rotate_count_(0),
          now_pos_(now_pos),
          previous_pos_(previous_pos)
    {
    }

    /// @brief 初始化参数聚合结构体
    struct Initializer {  // NOLINT(*-pro-type-member-init)
        DataType mid_pos;
        DataType high_pos;
        DataType low_pos;
        DataType now_pos;
        DataType previous_pos;
    };

    /**
     * @brief 通过初始化参数结构体构造多圈记录器
     * @param initializer 初始化参数
     */
    explicit constexpr MotorRotateRecorder(const Initializer& initializer) noexcept
        : MotorRotateRecorder(initializer.mid_pos,
                              initializer.high_pos,
                              initializer.low_pos,
                              initializer.now_pos,
                              initializer.previous_pos)
    {
    }

    /**
     * @brief 获取转子累计旋转圈数
     * @return 旋转圈数
     */
    [[nodiscard]] int_fast32_t getRotateCount() const noexcept
    {
        return rotate_count_;
    }

    /**
     * @brief 记录当前转子位置，更新多圈计数
     * @param now_pos 当前位置值
     */
    void record(const DataType now_pos) noexcept
    {
        now_pos_ = now_pos;

        if (previous_pos_ >= high_pos_ && now_pos <= mid_pos_) {
            ++rotate_count_;
        }
        else if (previous_pos_ <= low_pos_ && now_pos >= mid_pos_) {
            --rotate_count_;
        }

        previous_pos_ = now_pos;
    }
};

/// @brief 电机组抽象基类（CRTP 模式），管理同组电机的批量发送
template<typename MotorType_, std::size_t motor_count_>
    requires requires(MotorType_ motor) {
        typename MotorType_::ControlType;
        typename MotorType_::SendDataType;
        { motor.getSendTarget_() } -> std::convertible_to<typename MotorType_::SendDataType>;
    }
class MotorGroup
{
public:
    using MotorType = MotorType_;
    static constexpr std::size_t motor_count = motor_count_;

    using ControlType = MotorType::ControlType;
    using SendDataType = MotorType::SendDataType;

    /// @brief 电机引用列表类型
    using MotorList = std::array<MotorType&, motor_count>;

    using Iterator = MotorList::iterator;             ///< 迭代器类型
    using ConstIterator = MotorList::const_iterator;  ///< 常量迭代器类型

private:
    MotorList motors_;                                 ///< 电机引用列表
    std::array<SendDataType, motor_count> targets_{};  ///< 各电机控制目标值数组

public:
    /**
     * @brief 通过电机引用列表构造电机组
     * @param motors 电机引用列表
     */
    explicit constexpr MotorGroup(const MotorList& motors) noexcept : motors_(motors) {}

private:
    /**
     * @brief 辅助函数：将变参电机转化为 MotorList
     * @tparam Motors 电机类型
     * @return 电机引用列表
     */
    template<typename... Motors>
    constexpr auto initMotorsHelper_(Motors&&... motors) noexcept
    {
        static_assert(sizeof...(Motors) == motor_count,
                      "The count of parameter package 'motors' should be equals to 'motor_count'");
        return MotorList{std::forward<Motors>(motors)...};
    }

public:
    /**
     * @brief 通过变参电机引用构造电机组
     * @tparam Motors 电机类型
     * @param motors 电机引用
     */
    template<typename... Motors>
        requires(std::is_same_v<MotorType, std::decay_t<Motors>> && ...)
    explicit constexpr MotorGroup(Motors&&... motors) noexcept
        : motors_(initMotorsHelper_(std::forward<Motors>(motors)...))
    {
    }

    virtual ~MotorGroup() = default;

    Iterator begin() noexcept
    {
        return motors_.begin();
    }
    Iterator end() noexcept
    {
        return motors_.end();
    }
    ConstIterator begin() const noexcept
    {
        return motors_.begin();
    }
    ConstIterator end() const noexcept
    {
        return motors_.end();
    }
    ConstIterator cbegin() const noexcept
    {
        return motors_.cbegin();
    }
    ConstIterator cend() const noexcept
    {
        return motors_.cend();
    }

private:
    /**
     * @brief 检查索引是否有效（1-based）
     * @param index 索引值
     * @return 索引有效返回 true
     */
    static constexpr bool isValidIndex_(const std::size_t index) noexcept
    {
        return index >= 1 && index <= motor_count;
    }

public:
    /**
     * @brief 通过索引获取电机引用（1-based）
     * @param index 电机索引（1 ~ motor_count）
     * @return 电机引用
     */
    MotorType& operator[](const std::size_t index) noexcept
    {
        EMDEVIF_ASSERT(isValidIndex_(index), "The index should be in range [1, motor_count]");
        return motors_[index - 1];
    }
    /**
     * @brief 通过索引获取电机指针（1-based）
     * @param index 电机索引（1 ~ motor_count）
     * @return 电机指针，索引无效返回 nullptr
     */
    MotorType* at(const std::size_t index) noexcept
    {
        if (isValidIndex_(index)) {
            return &motors_[index - 1];
        }
        return nullptr;
    }
    /**
     * @brief 通过索引获取常量电机引用（1-based）
     * @param index 电机索引（1 ~ motor_count）
     * @return 常量电机引用
     */
    const MotorType& operator[](const std::size_t index) const noexcept
    {
        EMDEVIF_ASSERT(isValidIndex_(index), "The index should be in range [1, motor_count]");
        return motors_[index - 1];
    }
    /**
     * @brief 通过索引获取常量电机指针（1-based）
     * @param index 电机索引（1 ~ motor_count）
     * @return 常量电机指针，索引无效返回 nullptr
     */
    const MotorType* at(const std::size_t index) const noexcept
    {
        if (isValidIndex_(index)) {
            return &motors_[index - 1];
        }
        return nullptr;
    }

protected:
    /**
     * @brief 发送控制目标值的实现（子类必须重写）
     * @param targets 各电机的控制目标值
     * @return 错误码
     */
    virtual emdevif::ErrorCode sendImpl(std::span<const SendDataType> targets) const noexcept = 0;

public:
    /**
     * @brief 收集所有电机的控制目标值并发送
     * @return 错误码
     */
    emdevif::ErrorCode send() noexcept
    {
        for (std::size_t i = 0; i < motor_count; ++i) {
            targets_[i] = motors_[i].getSendTarget_();
        }
        return sendImpl(targets_);
    }
};

}  // namespace rmdev

#endif  // !RMDEV_DEVICE_MODEL_MOTOR_HPP
