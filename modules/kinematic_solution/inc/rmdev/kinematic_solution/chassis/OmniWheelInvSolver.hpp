/**
 * @file OmniWheelInvSolver.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_KINEMATIC_SOLUTION_CHASSIS_OMNI_WHEEL_INV_SOLVER_HPP
#define RMDEV_KINEMATIC_SOLUTION_CHASSIS_OMNI_WHEEL_INV_SOLVER_HPP

#pragma once

#include <cmath>

#include "emdevif/core/concepts.hpp"
#include "rmdev/kinematic_solution/chassis/FourWheelChassisSolver.hpp"
#include "rmdev/math.hpp"
#include "rmdev/matrix.hpp"

namespace rmdev {

/**
 * 全向轮逆运动学解算类
 * @tparam Type 数据类型
 * @tparam wheel_num 底盘轮组数量（默认4个）
 */
template<emdevif::ArithmeticType Type, std::size_t wheel_num = 4>
class OmniWheelInvSolver;

/**
 * 全向轮逆运动学解算类 - 四轮轮组特化
 * @tparam Type 数据类型
 */
template<emdevif::ArithmeticType Type>
class OmniWheelInvSolver<Type, 4> : public FourWheelChassisSolver<OmniWheelInvSolver<Type, 4>, Type>
{
public:
    /**
     * 数据类型
     */
    using ScaleType = Type;

    OmniWheelInvSolver() = delete;

    friend class FourWheelChassisSolver<OmniWheelInvSolver<Type, 4>, Type>;

    /**
     * 通过底盘宽度和长度构造全向轮逆运动学解算器
     * @param width 底盘宽度
     * @param length 底盘长度
     */
    constexpr OmniWheelInvSolver(const ScaleType width, const ScaleType length) noexcept
        : FourWheelChassisSolver<OmniWheelInvSolver, ScaleType>(width, length)
    {
    }

    /**
     * 构造正方形底盘的全向轮逆运动学解算器
     * @param square_length 正方形底盘边长
     */
    explicit constexpr OmniWheelInvSolver(const ScaleType square_length) noexcept
        : OmniWheelInvSolver(square_length, square_length)
    {
    }

private:
    /**
     * @brief 全向轮逆运动学解算实现
     * @param speed 底盘目标速度向量
     * @return 四个轮组的速度
     */
    auto solve_impl(const ChassisTargetSpeed<Type>& speed) const -> ChassisWheelsSpeed<ScaleType>
    {
        const Matrix<ScaleType, 3, 1> target_speed{speed[WHEEL_VX], speed[WHEEL_VY], speed[WHEEL_OMEGA]};

        Matrix<ScaleType, 4, 1> wheel_speeds;

        wheel_speeds = traits_matrix * target_speed;
        wheel_speeds *= ScaleType(num::invSqrt2);

        return {.fl = wheel_speeds(1, 1), .bl = wheel_speeds(2, 1), .br = wheel_speeds(3, 1), .fr = wheel_speeds(4, 1)};
    }

    /* clang-format off */
    /// @brief 全向轮运动学特征矩阵（4x3），用于将底盘速度映射到轮组速度
    const Matrix<ScaleType, 4, 3> traits_matrix{{1, -1, -this->rx[WHEEL_FL] - this->ry[WHEEL_FL]},
                                                {1,  1,  this->rx[WHEEL_BL] - this->ry[WHEEL_BL]},
                                                {1, -1, -this->rx[WHEEL_BR] - this->ry[WHEEL_BR]},
                                                {1,  1,  this->rx[WHEEL_FR] - this->ry[WHEEL_FR]}};
    /* clang-format on */
};

}  // namespace rmdev

#endif  // !RMDEV_KINEMATIC_SOLUTION_CHASSIS_OMNI_WHEEL_INV_SOLVER_HPP
