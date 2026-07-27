/**
 * @file Matrix.cppm
 * @brief 矩阵运算
 */

module;

#include "rmdev/matrix.hpp"

export module rmdev.matrix;

export namespace rmdev {
using ::rmdev::SpecialMatrixTag;
using ::rmdev::calculateGenericMatrixDeterminant;
using ::rmdev::MatrixCouldMultiplied;
using ::rmdev::SquareMatrix;
}  // namespace rmdev

export namespace rmdev {
using ::rmdev::Matrix;
}  // namespace rmdev
