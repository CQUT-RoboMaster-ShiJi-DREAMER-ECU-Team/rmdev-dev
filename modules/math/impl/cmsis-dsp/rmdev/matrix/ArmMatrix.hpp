/**
 * @file ArmMatrix.hpp
 * @brief CMSIS-DSP 矩阵运算封装
 */

#pragma once

#include <cstddef>
#include <cstring>

#include <type_traits>
#include <algorithm>
#include <iterator>
#include <memory>
#include <span>
#include <initializer_list>

#include "arm_math.h"

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"

#include "rmdev/matrix/matrix_base.hpp"
#include "rmdev/math.hpp"
#include "ArmMatrixTraits.hpp"

namespace rmdev {

template<typename Type, std::size_t row, std::size_t col>
    requires std::is_arithmetic_v<Type>
class ArmMatrix
{
public:
    template<typename U, std::size_t R, std::size_t C>
        requires std::is_arithmetic_v<U>
    friend class ArmMatrix;

    using ValueType = Type;

    static constexpr std::size_t rows() noexcept
    {
        return row;
    }

    static constexpr std::size_t cols() noexcept
    {
        return col;
    }

    constexpr ArmMatrix() noexcept
    {
        ArmMatrixTraits<Type>::init(&matrix_, row, col, data_);
    }

    constexpr ArmMatrix(const ArmMatrix& other) noexcept
    {
        std::ranges::copy(other.data_, data_);
        ArmMatrixTraits<Type>::init(&matrix_, row, col, data_);
    }

    constexpr ArmMatrix& operator=(const ArmMatrix& other) noexcept
    {
        if (this != &other) {
            std::ranges::copy(other.data_, data_);
        }
        return *this;
    }

    explicit constexpr ArmMatrix(const SpecialMatrixTag tag) noexcept
    {
        initSpecialMatrix(std::span{data_, std::size(data_)}, row, col, tag);
        ArmMatrixTraits<Type>::init(&matrix_, row, col, data_);
    }

    explicit constexpr ArmMatrix(const Type (&mat_data)[row * col]) noexcept
    {
        std::ranges::copy(mat_data, data_);
        ArmMatrixTraits<Type>::init(&matrix_, row, col, data_);
    }

    explicit constexpr ArmMatrix(const Type (&mat_data)[row][col]) noexcept
    {
        std::ranges::copy(std::addressof(mat_data[0][0]), std::addressof(mat_data[row - 1][col - 1]) + 1, data_);
        ArmMatrixTraits<Type>::init(&matrix_, row, col, data_);
    }

    constexpr ArmMatrix(std::initializer_list<Type> mat_data) noexcept
    {
        std::ranges::copy(mat_data, data_);
        ArmMatrixTraits<Type>::init(&matrix_, row, col, data_);
    }

    constexpr ArmMatrix& operator=(std::initializer_list<Type> mat_data) noexcept
    {
        std::ranges::copy(mat_data, data_);
        return *this;
    }

    constexpr ArmMatrix(std::initializer_list<std::initializer_list<Type>> mat_data) noexcept
    {
        auto* dst = data_;
        for (const auto& row_data : mat_data) {
            dst = std::ranges::copy(row_data, dst).out;
        }
        ArmMatrixTraits<Type>::init(&matrix_, row, col, data_);
    }

    constexpr ArmMatrix& operator=(std::initializer_list<std::initializer_list<Type>> mat_data) noexcept
    {
        auto* dst = data_;
        for (const auto& row_data : mat_data) {
            dst = std::ranges::copy(row_data, dst).out;
        }
        return *this;
    }

    [[nodiscard]] constexpr Type* at(std::size_t r, std::size_t c) noexcept
    {
        if (r >= row || c >= col) {
            return nullptr;
        }
        return &data_[r * col + c];
    }
    [[nodiscard]] constexpr const Type* at(std::size_t r, std::size_t c) const noexcept
    {
        if (r >= row || c >= col) {
            return nullptr;
        }
        return &data_[r * col + c];
    }

    [[nodiscard]] constexpr Type& operator()(std::size_t r, std::size_t c) noexcept
    {
        EMDEVIF_ASSERT(r < row && c < col, "Matrix index out of bounds");
        return data_[r * col + c];
    }
    [[nodiscard]] constexpr const Type& operator()(std::size_t r, std::size_t c) const noexcept
    {
        EMDEVIF_ASSERT(r < row && c < col, "Matrix index out of bounds");
        return data_[r * col + c];
    }

    /**
     * 计算行列式
     *
     * @attention 对于矩阵维数 >= 5 的矩阵，计算行列式时将会使用动态内存分配，请注意传递合适的内存分配器
     * （默认的 std::allocator 通常不适用于嵌入式开发）；对于维数 <= 4
     * 的矩阵，不会使用动态内存分配，无需关心内存分配器。
     * @tparam Allocator 内存分配器类型
     * @param allocator 分配器
     * @return 行列式计算结果
     */
    template<typename Allocator = std::allocator<Type>>
    [[nodiscard]] constexpr Type determinant(const Allocator& allocator = Allocator()) const noexcept
    {
        return calculateGenericMatrixDeterminant(std::span{data_, std::size(data_)}, row, col, allocator);
    }

    constexpr void fill(Type value) noexcept
    {
        std::ranges::fill(data_, value);
    }

    constexpr void clear() noexcept
    {
        std::ranges::fill(data_, Type{});
    }

    [[nodiscard]] constexpr bool operator==(const ArmMatrix& other) const noexcept
    {
        return std::ranges::equal(other.data_, data_);
    }

    [[nodiscard]] constexpr bool isApprox(const ArmMatrix& other, Type error) const noexcept
    {
        return std::ranges::equal(other.data_, data_, [error](Type a, Type b) noexcept -> bool {
            return weakEqu(a, b, error);
        });
    }

    ArmMatrix& operator+=(const ArmMatrix& other) noexcept
    {
        ArmMatrixTraits<Type>::add(&matrix_, &other.matrix_, &matrix_);
        return *this;
    }

    ArmMatrix& operator-=(const ArmMatrix& other) noexcept
    {
        ArmMatrixTraits<Type>::sub(&matrix_, &other.matrix_, &matrix_);
        return *this;
    }

    ArmMatrix& operator*=(Type scalar) noexcept
    {
        ArmMatrixTraits<Type>::scale(&matrix_, scalar, &matrix_);
        return *this;
    }

    ArmMatrix& operator*=(const ArmMatrix<Type, col, col>& other) noexcept
    {
        ArmMatrix temp;
        ArmMatrixTraits<Type>::mult(&matrix_, &other.matrix_, &temp.matrix_);
        std::memcpy(data_, temp.data_, sizeof(data_));
        return *this;
    }

    ArmMatrix& operator/=(Type scalar) noexcept
    {
        ArmMatrixTraits<Type>::scale(&matrix_, static_cast<Type>(1) / scalar, &matrix_);
        return *this;
    }

    ArmMatrix& operator/=(const ArmMatrix<Type, col, col>& other) noexcept
    {
        ArmMatrix<Type, col, col> inv_other = other.inverse();
        ArmMatrix temp;
        ArmMatrixTraits<Type>::mult(&matrix_, &inv_other.matrix_, &temp.matrix_);
        std::memcpy(data_, temp.data_, sizeof(data_));
        return *this;
    }

    [[nodiscard]] ArmMatrix operator+(const ArmMatrix& other) const noexcept
    {
        ArmMatrix result;
        ArmMatrixTraits<Type>::add(&matrix_, &other.matrix_, &result.matrix_);
        return result;
    }

    [[nodiscard]] ArmMatrix operator-(const ArmMatrix& other) const noexcept
    {
        ArmMatrix result;
        ArmMatrixTraits<Type>::sub(&matrix_, &other.matrix_, &result.matrix_);
        return result;
    }

    [[nodiscard]] ArmMatrix operator*(Type scalar) const noexcept
    {
        ArmMatrix result;
        ArmMatrixTraits<Type>::scale(&matrix_, scalar, &result.matrix_);
        return result;
    }

    [[nodiscard]] friend ArmMatrix operator*(Type scalar, const ArmMatrix& mat) noexcept
    {
        ArmMatrix result;
        ArmMatrixTraits<Type>::scale(&mat.matrix_, scalar, &result.matrix_);
        return result;
    }

    template<std::size_t col2>
    [[nodiscard]] ArmMatrix<Type, row, col2> operator*(const ArmMatrix<Type, col, col2>& rhs) noexcept
    {
        ArmMatrix<Type, row, col2> result;
        ArmMatrixTraits<Type>::mult(&matrix_, &rhs.matrix_, &result.matrix_);
        return result;
    }

    [[nodiscard]] ArmMatrix operator/(Type scalar) const noexcept
    {
        ArmMatrix result;
        ArmMatrixTraits<Type>::scale(&matrix_, static_cast<Type>(1) / scalar, &result.matrix_);
        return result;
    }

    [[nodiscard]] ArmMatrix operator/(const ArmMatrix<Type, col, col>& other) const noexcept
    {
        ArmMatrix<Type, col, col> inv_other = other.inverse();
        ArmMatrix result;
        ArmMatrixTraits<Type>::mult(&matrix_, &inv_other.matrix_, &result.matrix_);
        return result;
    }

    [[nodiscard]] friend ArmMatrix operator/(Type scalar, const ArmMatrix& mat) noexcept
        requires SquareMatrix<row, col>
    {
        ArmMatrix result = mat.inverse();
        ArmMatrixTraits<Type>::scale(&result.matrix_, scalar, &result.matrix_);
        return result;
    }

    [[nodiscard]] ArmMatrix<Type, col, row> transpose() const noexcept
    {
        ArmMatrix<Type, col, row> result;
        ArmMatrixTraits<Type>::trans(&matrix_, &result.matrix_);
        return result;
    }

    void transposeInPlace() noexcept
        requires SquareMatrix<row, col>
    {
        ArmMatrix temp = *this;
        ArmMatrixTraits<Type>::trans(&temp.matrix_, &matrix_);
    }

    [[nodiscard]] ArmMatrix inverse() const noexcept
        requires SquareMatrix<row, col>
    {
        ArmMatrix result;
        ArmMatrix copy = *this;
        ArmMatrixTraits<Type>::inverse(&copy.matrix_, &result.matrix_);
        return result;
    }

    void inverseInPlace() noexcept
        requires SquareMatrix<row, col>
    {
        ArmMatrix copy = *this;
        ArmMatrixTraits<Type>::inverse(&copy.matrix_, &matrix_);
    }

private:
    alignas(Type) Type data_[row * col]{};
    typename ArmMatrixTraits<Type>::ArmMatrixInstance matrix_{};
};

}  // namespace rmdev
