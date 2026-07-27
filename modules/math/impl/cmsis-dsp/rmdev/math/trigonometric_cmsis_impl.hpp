/**
 * @file trigonometric_cmsis_impl.hpp
 * @brief 三角学 CMSIS-DSP 特化的加速实现
 */

#pragma once

#include <utility>

#include "arm_math.h"

namespace rmdev {

template<>
inline float sin<float>(const float x)
{
    return arm_sin_f32(x);
}

template<>
inline float cos<float>(const float x) noexcept
{
    return arm_cos_f32(x);
}

template<>
inline std::pair<float, float> sin_cos<float>(const float x) noexcept
{
    float sin_out, cos_out;
    arm_sin_cos_f32(x, &sin_out, &cos_out);
    return {sin_out, cos_out};
}

template<>
inline void sin_cos<float>(const float x, float& sin_out, float& cos_out) noexcept
{
    arm_sin_cos_f32(x, &sin_out, &cos_out);
}

}  // namespace rmdev
