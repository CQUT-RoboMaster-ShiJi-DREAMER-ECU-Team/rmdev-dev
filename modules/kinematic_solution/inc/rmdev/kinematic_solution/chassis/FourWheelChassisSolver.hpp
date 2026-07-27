/**
 * @file FourWheelChassisSolver.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_KINEMATIC_SOLUTION_CHASSIS_FOUR_WHEEL_CHASSIS_SOLVER_HPP
#define RMDEV_KINEMATIC_SOLUTION_CHASSIS_FOUR_WHEEL_CHASSIS_SOLVER_HPP

#include <cstdint>

#include <array>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/concepts.hpp"
#include "emdevif/core/error_handler.hpp"
namespace rmdev {

/**
 * 底盘轮组的编号
 */
enum ChassisWheelsIndex : std::uint_fast8_t {
    WHEEL_FL = 0U,  ///< 左前轮
    WHEEL_BL,       ///< 左后轮
    WHEEL_BR,       ///< 右后轮
    WHEEL_FR        ///< 右前轮
};

/**
 * 底盘轮组速度向量
 * @tparam Type 数据类型
 */
template<emdevif::ArithmeticType Type>
struct ChassisWheelsSpeed {
    Type fl;  ///< 左前轮
    Type bl;  ///< 左后轮
    Type br;  ///< 右后轮
    Type fr;  ///< 右前轮

    /**
     * @brief 按轮组索引获取速度值
     * @param n 轮组索引（WHEEL_FL / WHEEL_BL / WHEEL_BR / WHEEL_FR）
     * @return 对应轮组的速度常量引用
     */
    constexpr const Type& operator[](const std::size_t n) const noexcept
    {
        EMDEVIF_ASSERT(n <= WHEEL_FR, "out of range!");

        switch (n) {
        case WHEEL_FL:
            return fl;
        case WHEEL_BL:
            return bl;
        case WHEEL_BR:
            return br;
        case WHEEL_FR:
        default:
            return fr;
        }
    }

    /**
     * @overload
     * @return 对应分量的引用
     */
    constexpr Type& operator[](const std::size_t n) noexcept
    {
        return static_cast<const ChassisWheelsSpeed*>(this)->operator[](n);
    }
};

/**
 * 底盘目标速度向量的索引
 */
enum ChassisTargetSpeedIndex : std::uint_fast8_t {
    WHEEL_VX = 0,  ///< 前进速度
    WHEEL_VY,      ///< 侧向速度
    WHEEL_OMEGA    ///< 旋转角速度
};

/**
 * 底盘目标速度向量
 * @tparam Type 数据类型
 * @note 该向量包含三个分量：前进速度、侧向速度和旋转角速度，构成右手系
 */
template<emdevif::ArithmeticType Type>
struct ChassisTargetSpeed {
    Type vx;     ///< 前进速度
    Type vy;     ///< 侧向速度
    Type omega;  ///< 旋转角速度

    /**
     * @brief 按索引获取目标速度分量
     * @param n 分量索引（WHEEL_VX / WHEEL_VY / WHEEL_OMEGA）
     * @return 对应分量的常量引用
     */
    constexpr const Type& operator[](const std::size_t n) const noexcept
    {
        EMDEVIF_ASSERT(n <= WHEEL_OMEGA, "out of range!");

        switch (n) {
        case WHEEL_VX:
            return vx;
        case WHEEL_VY:
            return vy;
        case WHEEL_OMEGA:
        default:
            return omega;
        }
    }

    /**
     * @overload
     * @return 对应分量的引用
     */
    constexpr Type& operator[](const std::size_t n) noexcept
    {
        return static_cast<const ChassisTargetSpeed*>(this)->operator[](n);
    }
};

/**
 * 四轮底盘解算接口（使用 CRTP 模式）
 * @tparam SolverType 子类类型
 * @tparam Type SolverType 的数据类型
 */
template<class SolverType, emdevif::ArithmeticType Type>
class FourWheelChassisSolver
{
private:
    using ScaleType_ = Type;  ///< 数据类型

public:
    /**
     * 解算底盘轮组速度
     * @param speed 底盘目标速度向量
     * @return 底盘轮组速度向量
     */
    auto solve(const ChassisTargetSpeed<ScaleType_>& speed) const -> ChassisWheelsSpeed<ScaleType_>
    {
        return static_cast<const SolverType*>(this)->solve_impl(speed);
    }

    FourWheelChassisSolver() = delete;

protected:
    /**
     * 根据底盘长宽构造四轮底盘解算器
     * @param width 底盘宽度
     * @param length 底盘长度
     */
    constexpr FourWheelChassisSolver(const ScaleType_ width, const ScaleType_ length) noexcept
        : width_(width), length_(length)
    {
    }

    FourWheelChassisSolver(const FourWheelChassisSolver&) = default;
    FourWheelChassisSolver(FourWheelChassisSolver&&) = default;
    ~FourWheelChassisSolver() = default;

    const ScaleType_ width_;   ///< 底盘宽度
    const ScaleType_ length_;  ///< 底盘长度

    // 算法参考浙江大学的开源文档：
    // https://zju-helloworld.github.io/Wiki/%E7%BB%84%E4%BB%B6%E8%AF%B4%E6%98%8E%EF%BC%88%E6%97%A7%EF%BC%89/%E6%9C%BA%E5%99%A8%E4%BA%BA%E9%80%9A%E7%94%A8%E7%BB%84%E4%BB%B6/%E7%AE%97%E6%B3%95/%E5%BA%95%E7%9B%98%E9%80%86%E8%BF%90%E5%8A%A8%E5%AD%A6%E8%A7%A3%E7%AE%97/

    /* clang-format off */
    /// 各个轮组对旋转中心的位失的 x 方向的分量
    const std::array<ScaleType_, 4> rx{ length_ / ScaleType_(2),
                                       -length_ / ScaleType_(2),
                                       -length_ / ScaleType_(2),
                                        length_ / ScaleType_(2)};
    /// 各个轮组对旋转中心的位失的 y 方向的分量
    const std::array<ScaleType_, 4> ry{ width_ / ScaleType_(2),
                                        width_ / ScaleType_(2),
                                       -width_ / ScaleType_(2),
                                       -width_ / ScaleType_(2)};
    /* clang-format on */
};

}  // namespace rmdev

#endif  // !RMDEV_KINEMATIC_SOLUTION_CHASSIS_FOUR_WHEEL_CHASSIS_SOLVER_HPP
