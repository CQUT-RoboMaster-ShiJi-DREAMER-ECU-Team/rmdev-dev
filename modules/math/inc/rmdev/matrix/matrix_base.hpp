/**
 * @file matrix_base.hpp
 * @brief 矩阵基本声明
 */

#pragma once

#include <cstdint>

#include <algorithm>
#include <span>
#include <vector>

namespace rmdev {

/**
 * 特殊矩阵标签
 */
enum class SpecialMatrixTag : uint8_t {
    Normal = 0U,  ///< 普通矩阵
    E,            ///< 单位矩阵（必须是方阵才有意义，否则无效）
    One,          ///< 每个元素全是 1 的矩阵
    Zero          ///< 每个元素全是 0 的矩阵
};  // todo 可以考虑实现：上、下三角矩阵

/**
 * 通过特殊矩阵标签初始化矩阵
 * @tparam Type 类型
 * @param[out] data 矩阵数据
 * @param row 行数
 * @param col 列数
 * @param tag 特殊矩阵标签
 */
template<typename Type>
constexpr void initSpecialMatrix(std::span<Type> data,
                                 const std::size_t row,
                                 const std::size_t col,
                                 const SpecialMatrixTag tag) noexcept
{
    if (row == col) {
        switch (tag) {
        case SpecialMatrixTag::E:
            for (std::size_t i = 0; i < row; ++i) {
                data[i * col + i] = static_cast<Type>(1);
            }
            break;
        case SpecialMatrixTag::One:
            std::ranges::fill(data, static_cast<Type>(1));
            break;
        default:
            break;
        }
    }
    else {
        switch (tag) {
        case SpecialMatrixTag::One:
            std::ranges::fill(data, static_cast<Type>(1));
            break;
        default:
            break;
        }
    }
}

namespace detail {

/**
 * 对整数类型矩阵，使用 Bareiss 无分数消元算法计算行列式。
 * 通过精确整除避免浮点误差，中间结果保持整数，时间复杂度 O(n³)。
 * @tparam T 矩阵元素类型（满足 std::is_integral_v）
 * @tparam Allocator 分配器类型
 * @param data 矩阵数据（行优先，方阵）
 * @param row 行数
 * @param allocator 分配器
 * @return 行列式的值，row==0 时返回 1
 */
template<typename T, typename Allocator>
constexpr T calculateGenericMatrixDeterminantHigherDimension(std::span<const T> data,
                                                             const std::size_t row,
                                                             const Allocator& allocator) noexcept
    requires std::is_integral_v<T>
{
    const std::size_t n = row;
    std::vector<T> m(data.begin(), data.end(), allocator);

    T sign = T{1};
    T prev = T{1};

    for (std::size_t k = 0; k < n - 1; ++k) {
        if (m[k * n + k] == T{0}) {
            bool found = false;
            for (std::size_t i = k + 1; i < n; ++i) {
                if (m[i * n + k] != T{0}) {
                    for (std::size_t j = 0; j < n; ++j) {
                        T tmp = m[k * n + j];
                        m[k * n + j] = m[i * n + j];
                        m[i * n + j] = tmp;
                    }
                    sign = -sign;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return T{0};
            }
        }

        T pivot = m[k * n + k];

        for (std::size_t i = k + 1; i < n; ++i) {
            for (std::size_t j = k + 1; j < n; ++j) {
                m[i * n + j] = (m[i * n + j] * pivot - m[i * n + k] * m[k * n + j]) / prev;
            }
        }

        prev = pivot;
    }

    return sign * m[(n - 1) * n + (n - 1)];
}
/**
 * @overload
 *
 * 对浮点类型矩阵，使用部分主元 LU 分解计算行列式。
 * 通过行交换保证数值稳定性，时间复杂度 O(n³)。
 * @tparam T 矩阵元素类型（满足 std::is_floating_point_v）
 * @tparam Allocator 分配器类型
 * @param data 矩阵数据（行优先，方阵）
 * @param row 行数
 * @param allocator 分配器
 * @return 行列式的值，row==0 时返回 1
 */
template<typename T, typename Allocator>
constexpr T calculateGenericMatrixDeterminantHigherDimension(std::span<const T> data,
                                                             const size_t row,
                                                             const Allocator& allocator) noexcept
    requires std::is_floating_point_v<T>
{
    const std::size_t n = row;
    std::vector<T> m(data.begin(), data.end(), allocator);

    T det = T{1};
    T sign = T{1};

    for (std::size_t k = 0; k < n; ++k) {
        std::size_t pivot_row = k;
        T max_abs = m[k * n + k];
        if (max_abs < T{0}) {
            max_abs = -max_abs;
        }

        for (std::size_t i = k + 1; i < n; ++i) {
            T val = m[i * n + k];
            T abs_val = val < T{0} ? -val : val;
            if (abs_val > max_abs) {
                max_abs = abs_val;
                pivot_row = i;
            }
        }

        if (max_abs == T{0}) {
            return T{0};
        }

        if (pivot_row != k) {
            for (std::size_t j = 0; j < n; ++j) {
                T tmp = m[k * n + j];
                m[k * n + j] = m[pivot_row * n + j];
                m[pivot_row * n + j] = tmp;
            }
            sign = -sign;
        }

        for (std::size_t i = k + 1; i < n; ++i) {
            T factor = m[i * n + k] / m[k * n + k];
            for (std::size_t j = k + 1; j < n; ++j) {
                m[i * n + j] -= factor * m[k * n + j];
            }
        }

        det *= m[k * n + k];
    }

    return sign * det;
}

}  // namespace detail

template<typename T, typename Allocator = std::allocator<T>>
constexpr T calculateGenericMatrixDeterminant(std::span<const T> data,
                                              const std::size_t row,
                                              const std::size_t col,
                                              const Allocator& allocator = Allocator()) noexcept
{
    if (row != col) {
        return T{};
    }

    if (row == 0) {
        return T{};
    }
    if (row == 1) {
        return data[0];
    }
    if (row == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }
    if (row == 3) {
        // clang-format off
        return   data[0] * (data[4] * data[8] - data[5] * data[7])
               - data[1] * (data[3] * data[8] - data[5] * data[6])
               + data[2] * (data[3] * data[7] - data[4] * data[6]);
        // clang-format on
    }
    if (row == 4) {
        // clang-format off
        return   data[0] * (  data[5] * (data[10] * data[15] - data[11] * data[14])
                            - data[6] * (data[ 9] * data[15] - data[11] * data[13])
                            + data[7] * (data[ 9] * data[14] - data[10] * data[13]))
               - data[1] * (  data[4] * (data[10] * data[15] - data[11] * data[14])
                            - data[6] * (data[ 8] * data[15] - data[11] * data[12])
                            + data[7] * (data[ 8] * data[14] - data[10] * data[12]))
               + data[2] * (  data[4] * (data[ 9] * data[15] - data[11] * data[13])
                            - data[5] * (data[ 8] * data[15] - data[11] * data[12])
                            + data[7] * (data[ 8] * data[13] - data[ 9] * data[12]))
               - data[3] * (  data[4] * (data[ 9] * data[14] - data[10] * data[13])
                            - data[5] * (data[ 8] * data[14] - data[10] * data[12])
                            + data[6] * (data[ 8] * data[13] - data[ 9] * data[12]));
        // clang-format on
    }

    return detail::calculateGenericMatrixDeterminantHigherDimension(data, row, allocator);
}

/**
 * 矩阵可以相乘的概念
 * @tparam row1 乘号左边的矩阵的行数
 * @tparam col1 乘号左边的矩阵的列数
 * @tparam row2 乘号右边的矩阵的行数
 * @tparam col2 乘号右边的矩阵的列数
 */
template<std::size_t row1, std::size_t col1, std::size_t row2, std::size_t col2>
concept MatrixCouldMultiplied = (col1 == row2);

/**
 * 方阵概念
 * @tparam row 矩阵的行数
 * @tparam col 矩阵的列数
 */
template<std::size_t row, std::size_t col>
concept SquareMatrix = (row == col);

}  // namespace rmdev
