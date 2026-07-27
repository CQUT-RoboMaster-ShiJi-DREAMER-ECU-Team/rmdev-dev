/**
 * @file matrix.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_MATRIX_HPP
#define RMDEV_MATRIX_HPP

#include "rmdev/matrix/matrix_base.hpp"

#ifdef RMDEV_MATRIX_USE_CMSIS_DSP

#include "rmdev/matrix/arm_matrix.hpp"

namespace rmdev {

template<typename Type, std::size_t row, std::size_t col>
    requires emdevif::ArithmeticType<Type>
using Matrix = rmdev::ArmMatrix<Type, row, col>;

}  // namespace rmdev

#endif  // RMDEV_MATRIX_USE_CMSIS_DSP

// todo 待增加 MutableMatrix 与 DynamicMatrix

namespace rmdev {

using Matrix2f = Matrix<float, 2, 2>;
using Matrix3f = Matrix<float, 3, 3>;
using Matrix4f = Matrix<float, 4, 4>;

template<typename T, std::size_t dimension>
using ColVector = Matrix<T, dimension, 1>;

template<typename T, std::size_t dimension>
using RowVector = Matrix<T, 1, dimension>;

using ColVector2f = ColVector<float, 2>;
using ColVector3f = ColVector<float, 3>;
using ColVector4f = ColVector<float, 4>;

using RowVector2f = RowVector<float, 2>;
using RowVector3f = RowVector<float, 3>;
using RowVector4f = RowVector<float, 4>;

}  // namespace rmdev

#endif  // !RMDEV_MATRIX_HPP
