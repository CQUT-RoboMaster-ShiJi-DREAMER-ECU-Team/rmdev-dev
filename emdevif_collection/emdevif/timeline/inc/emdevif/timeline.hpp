/**
 * @file timeline.hpp
 * @brief 时间戳
 */

#pragma once
#ifndef EMDEVIF_TIMELINE_HPP
#define EMDEVIF_TIMELINE_HPP

#include <cstdint>

#include <type_traits>

#include "emdevif/core/simplify_decl_macros.hpp"

namespace emdevif {

/// 时间点类型
using BaseTimePoint = uint64_t;

namespace user_impl::timeline {
#if (!EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS)
BaseTimePoint getMicroseconds() noexcept;
BaseTimePoint getMilliseconds() noexcept = EMDEVIF_REASON_DELETE(
    "Macro `EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS` was false, so this function deleted. Please usage "
    "`getMicroseconds` instead");
#else
BaseTimePoint getMicroseconds() noexcept = EMDEVIF_REASON_DELETE(
    "Macro `EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS` was true, so this function deleted. Please usage "
    "`getMilliseconds` instead");
BaseTimePoint getMilliseconds() noexcept;
#endif
}  // namespace user_impl::timeline

/**
 * 时间线类
 */
class Timeline
{
public:
    /**
     * 时间线提供的最高精度
     */
    enum class BasicAccuracy : uint_fast8_t {
        Microseconds = 0,  ///< 微秒
        Milliseconds       ///< 毫秒
    };

    /// 当前时间线能够提供的最高精度
    static constexpr auto basic_accuracy =
        (EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS ? BasicAccuracy::Milliseconds : BasicAccuracy::Microseconds);

    /**
     * 获取从初始化完成到当前的时间（以微秒为单位）
     * @return 当前时间
     */
    static BaseTimePoint getMicroseconds() noexcept
#if (!EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS)
    {
        return ::emdevif::user_impl::timeline::getMicroseconds();
    }
#else
        = EMDEVIF_REASON_DELETE(
            "Macro `EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS` was true, so this function deleted. Please usage "
            "`getMilliseconds` instead");
#endif

    /**
     * 获取从初始化完成到当前的时间（以毫秒为单位）
     * @return 当前时间
     */
    static BaseTimePoint getMilliseconds() noexcept
#if (EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS)
    {
        return ::emdevif::user_impl::timeline::getMilliseconds();
    }
#else
        = EMDEVIF_REASON_DELETE(
            "Macro `EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS` was false, so this function deleted. Please usage "
            "`getMicroseconds` instead");
#endif

    /**
     * 阻塞式延时一段时间
     * @param delay_time_us 要延时的时间（以微秒为单位）
     */
    static void pauseDelayUs(const BaseTimePoint delay_time_us) noexcept
#if (!EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS)
    {
        const volatile auto begin_time = getMicroseconds();
        while (getMicroseconds() - begin_time < delay_time_us) {
        }
    }
#else
        = EMDEVIF_REASON_DELETE(
            "Macro `EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS` was true, the accuracy of time source could not support "
            "delay in microseconds. So this function deleted, please usage `pauseDelayMs` instead");
#endif

    /**
     * 阻塞式延时一段时间
     * @param delay_time_ms 要延时的时间（以毫秒为单位）
     */
    static void pauseDelayMs(const BaseTimePoint delay_time_ms) noexcept
    {
#if (!EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS)
        pauseDelayUs(delay_time_ms * 1000U);
#else
        const volatile auto begin_time = getMilliseconds();
        while (getMilliseconds() - begin_time < delay_time_ms) {
        }
#endif
    }

public:
    constexpr Timeline() noexcept = default;
    constexpr Timeline(const Timeline&) noexcept = default;

    explicit constexpr Timeline(const BaseTimePoint& init_value) noexcept : store_timeline_(init_value) {}

    Timeline& operator=(const BaseTimePoint& init_value) noexcept
    {
        store_timeline_ = init_value;
        return *this;
    }

    /**
     * 更新存储的时间
     */
    void update() noexcept
    {
        store_timeline_ =
#if (EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS)
            getMilliseconds();
#else
            getMicroseconds();
#endif
    }

    /**
     * 获取存储的时间戳
     * @return 时间点值（以微秒为单位）
     */
    BaseTimePoint operator()() const noexcept
    {
        return store_timeline_;
    }

    template<typename OtherType>
        requires(std::is_convertible_v<BaseTimePoint, OtherType>)
    operator OtherType() noexcept  // NOLINT
    {
        return static_cast<OtherType>(store_timeline_);
    }

private:
    BaseTimePoint store_timeline_;  ///< 存储的时间点值（以微秒为单位）
};

/**
 * 间隔时间类
 * @tparam ConvertType 获取值时转化的类型
 */
template<typename ConvertType = float>
class Duration
{
public:
    static constexpr auto basic_accuracy = Timeline::basic_accuracy;  ///< 时间线的最高精度

    constexpr Duration() noexcept = default;
    constexpr Duration(const Duration&) noexcept = default;

    template<typename OtherType>
        requires(requires(OtherType v) { Timeline{v}; })
    explicit constexpr Duration(const OtherType& last_time) noexcept : last_time_point_(last_time)
    {
    }

    /**
     * 获取间隔时间（以微秒为单位）
     * @return 间隔时间
     */
    ConvertType getMicroDuration() noexcept
#if (!EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS)
    {
        const auto now = Timeline::getMicroseconds();
        const auto ret = now - last_time_point_();
        last_time_point_ = now;
        return ret;
    }
#else
        = EMDEVIF_REASON_DELETE(
            "Macro `EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS` was true, the accuracy of time source could not support "
            "duration in microseconds. So this function deleted, please usage `getMilliDuration` instead");
#endif

    /**
     * 获取间隔时间（以毫秒为单位）
     * @return 间隔时间
     */
    ConvertType getMilliDuration() noexcept
    {
#if (!EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS)
        return getMicroDuration() / static_cast<ConvertType>(1000);
#else
        const auto now = Timeline::getMilliseconds();
        const auto ret = now - last_time_point_();
        last_time_point_ = now;
        return ret;
#endif
    }

    /**
     * 获取间隔时间（以秒为单位）
     * @return 间隔时间
     */
    ConvertType getSecondsDuration() noexcept
    {
        return getMilliDuration() / static_cast<ConvertType>(1000);
    }

    /**
     * 获取上一次时间的时间戳
     * @return 上一次的时间戳
     */
    ConvertType getLastTime() noexcept
    {
        return static_cast<ConvertType>(last_time_point_());
    }

    /**
     * 更新存储的时间戳
     */
    void update() noexcept
    {
        last_time_point_.update();
    }

private:
    Timeline last_time_point_;  ///< 上一次的时间戳
};

}  // namespace emdevif

#endif  // !EMDEVIF_TIMELINE_HPP
