#include "host_test_pch.hpp"

#if EMDEVIF_USE_MODULES
import rmdev.matrix;
#else
#include "rmdev/matrix.hpp"
#endif

using namespace rmdev;

namespace {

using M11 = Matrix<float, 1, 1>;
using M22 = Matrix<float, 2, 2>;
using M33 = Matrix<float, 3, 3>;
using M23 = Matrix<float, 2, 3>;
using M32 = Matrix<float, 3, 2>;
using M44 = Matrix<float, 4, 4>;
using M55d = Matrix<double, 5, 5>;
using M66d = Matrix<double, 6, 6>;

}  // anonymous namespace

// ============================================================
// Construction
// ============================================================
TEST(MatrixConstruction, DefaultIsZero)
{
    M22 m;
    for (std::size_t r = 0; r < 2; ++r) {
        for (std::size_t c = 0; c < 2; ++c) {
            EXPECT_FLOAT_EQ(*m.at(r, c), 0.0f);
        }
    }
}

TEST(MatrixConstruction, IdentityMatrix)
{
    M22 m(SpecialMatrixTag::E);
    EXPECT_FLOAT_EQ(m(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m(0, 1), 0.0f);
    EXPECT_FLOAT_EQ(m(1, 0), 0.0f);
    EXPECT_FLOAT_EQ(m(1, 1), 1.0f);
}

TEST(MatrixConstruction, OneMatrix)
{
    M23 m(SpecialMatrixTag::One);
    for (std::size_t r = 0; r < 2; ++r) {
        for (std::size_t c = 0; c < 3; ++c) {
            EXPECT_FLOAT_EQ(m(r, c), 1.0f);
        }
    }
}

TEST(MatrixConstruction, CopyConstructor)
{
    M22 a = {1, 2, 3, 4};
    M22 b(a);
    EXPECT_EQ(a, b);
    b(0, 0) = 99.0f;
    EXPECT_NE(a, b);
}

TEST(MatrixConstruction, FromInitializerList)
{
    M22 m = {1, 2, 3, 4};
    EXPECT_FLOAT_EQ(m(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m(0, 1), 2.0f);
    EXPECT_FLOAT_EQ(m(1, 0), 3.0f);
    EXPECT_FLOAT_EQ(m(1, 1), 4.0f);
}

TEST(MatrixConstruction, FromNestedInitializerList)
{
    M22 m = {{1, 2}, {3, 4}};
    EXPECT_FLOAT_EQ(m(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m(0, 1), 2.0f);
    EXPECT_FLOAT_EQ(m(1, 0), 3.0f);
    EXPECT_FLOAT_EQ(m(1, 1), 4.0f);
}

TEST(MatrixConstruction, From1DCArray)
{
    const float data[4] = {1, 2, 3, 4};
    M22 m(data);
    M22 expected = {1, 2, 3, 4};
    EXPECT_EQ(m, expected);
}

TEST(MatrixConstruction, From2DCArray)
{
    const float data[2][2] = {{1, 2}, {3, 4}};
    M22 m(data);
    M22 expected = {1, 2, 3, 4};
    EXPECT_EQ(m, expected);
}

TEST(MatrixConstruction, Assignment)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    a = b;
    EXPECT_EQ(a, b);
}

// ============================================================
// Element Access: at() and operator() (0-indexed)
// ============================================================
TEST(MatrixAccess, AtFirstElement)
{
    M22 m = {10, 20, 30, 40};
    EXPECT_NE(m.at(0, 0), nullptr);
    EXPECT_FLOAT_EQ(*m.at(0, 0), 10.0f);
}

TEST(MatrixAccess, AtLastElement)
{
    M33 m = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_FLOAT_EQ(*m.at(2, 2), 9.0f);
}

TEST(MatrixAccess, AtOutOfBoundsReturnsNull)
{
    M22 m = {1, 2, 3, 4};
    EXPECT_EQ(m.at(0, 2), nullptr);
    EXPECT_EQ(m.at(2, 0), nullptr);
    EXPECT_EQ(m.at(2, 2), nullptr);
}

TEST(MatrixAccess, ParenOperator)
{
    M22 m = {10, 20, 30, 40};
    EXPECT_FLOAT_EQ(m(0, 0), 10.0f);
    EXPECT_FLOAT_EQ(m(0, 1), 20.0f);
    EXPECT_FLOAT_EQ(m(1, 0), 30.0f);
    EXPECT_FLOAT_EQ(m(1, 1), 40.0f);
}

TEST(MatrixAccess, ParenOperatorModifies)
{
    M22 m = {1, 2, 3, 4};
    m(0, 0) = 99.0f;
    EXPECT_FLOAT_EQ(m(0, 0), 99.0f);
}

TEST(MatrixAccess, AtConst)
{
    M22 m = {1, 2, 3, 4};
    const auto& cm = m;
    EXPECT_FLOAT_EQ(*cm.at(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(*cm.at(1, 1), 4.0f);
}

TEST(MatrixAccess, ParenConst)
{
    M22 m = {1, 2, 3, 4};
    const auto& cm = m;
    EXPECT_FLOAT_EQ(cm(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(cm(1, 1), 4.0f);
}

// ============================================================
// fill / clear
// ============================================================
TEST(MatrixModify, Fill)
{
    M23 m = {1, 2, 3, 4, 5, 6};
    m.fill(7.0f);
    for (std::size_t r = 0; r < 2; ++r) {
        for (std::size_t c = 0; c < 3; ++c) {
            EXPECT_FLOAT_EQ(m(r, c), 7.0f);
        }
    }
}

TEST(MatrixModify, Clear)
{
    M22 m = {1, 2, 3, 4};
    m.clear();
    for (std::size_t r = 0; r < 2; ++r) {
        for (std::size_t c = 0; c < 2; ++c) {
            EXPECT_FLOAT_EQ(m(r, c), 0.0f);
        }
    }
}

// ============================================================
// Comparison
// ============================================================
TEST(MatrixComparison, Equal)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {1, 2, 3, 4};
    EXPECT_EQ(a, b);
}

TEST(MatrixComparison, NotEqual)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {1, 2, 3, 5};
    EXPECT_NE(a, b);
}

TEST(MatrixComparison, IsApproxWithinTolerance)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {1.0001f, 2.0001f, 3.0001f, 4.0001f};
    EXPECT_TRUE(a.isApprox(b, 0.001f));
}

TEST(MatrixComparison, IsApproxOutsideTolerance)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {1.1f, 2.2f, 3.3f, 4.4f};
    EXPECT_FALSE(a.isApprox(b, 0.05f));
}

// ============================================================
// Arithmetic Correctness
// ============================================================
TEST(MatrixArithmetic, Add)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    M22 expected = {6, 8, 10, 12};
    EXPECT_EQ(a + b, expected);
}

TEST(MatrixArithmetic, Sub)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    M22 expected = {-4, -4, -4, -4};
    EXPECT_EQ(a - b, expected);
}

TEST(MatrixArithmetic, ScalarMul)
{
    M22 a = {1, 2, 3, 4};
    M22 expected = {2, 4, 6, 8};
    EXPECT_EQ(a * 2.0f, expected);
}

TEST(MatrixArithmetic, ScalarMulLeft)
{
    M22 a = {1, 2, 3, 4};
    M22 expected = {2, 4, 6, 8};
    EXPECT_EQ(2.0f * a, expected);
}

TEST(MatrixArithmetic, MatMul2x2)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    M22 expected = {19, 22, 43, 50};
    EXPECT_EQ(a * b, expected);
}

TEST(MatrixArithmetic, MatMul2x3x3x2)
{
    M23 a = {1, 2, 3, 4, 5, 6};
    M32 b = {1, 2, 3, 4, 5, 6};
    M22 result = a * b;
    M22 expected = {22, 28, 49, 64};
    EXPECT_EQ(result, expected);
}

TEST(MatrixArithmetic, ScalarDiv)
{
    M22 a = {2, 4, 6, 8};
    M22 expected = {1, 2, 3, 4};
    EXPECT_EQ(a / 2.0f, expected);
}

TEST(MatrixArithmetic, ScalarDivLeft)
{
    M22 a = {1, 2, 3, 4};
    M22 expected = {-4, 2, 3, -1};
    M22 result = 2.0f / a;
    EXPECT_TRUE(result.isApprox(expected, 1e-5f));
}

TEST(MatrixArithmetic, MatDiv)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    M22 expected = {3, -2, 2, -1};
    M22 result = a / b;
    EXPECT_TRUE(result.isApprox(expected, 1e-5f));
}

TEST(MatrixArithmetic, TransposeSquare)
{
    M22 a = {1, 2, 3, 4};
    M22 result = a.transpose();
    M22 expected = {1, 3, 2, 4};
    EXPECT_EQ(result, expected);
}

TEST(MatrixArithmetic, TransposeNonSquare)
{
    M23 a = {1, 2, 3, 4, 5, 6};
    M32 result = a.transpose();
    M32 expected = {1, 4, 2, 5, 3, 6};
    EXPECT_EQ(result, expected);
}

TEST(MatrixArithmetic, Inverse2x2)
{
    M22 a = {1, 2, 3, 4};
    M22 inv = a.inverse();
    M22 prod = a * inv;
    M22 eye(SpecialMatrixTag::E);
    EXPECT_TRUE(prod.isApprox(eye, 1e-5f));
}

TEST(MatrixArithmetic, Inverse3x3)
{
    M33 a = {1, 2, 3, 0, 1, 4, 5, 6, 0};
    M33 inv = a.inverse();
    M33 prod = a * inv;
    M33 eye(SpecialMatrixTag::E);
    EXPECT_TRUE(prod.isApprox(eye, 1e-5f));
}

TEST(MatrixArithmetic, InverseInPlace2x2)
{
    M22 a = {1, 2, 3, 4};
    a.inverseInPlace();
    M22 expected = {-2.0f, 1.0f, 1.5f, -0.5f};
    EXPECT_TRUE(a.isApprox(expected, 1e-5f));
}

TEST(MatrixArithmetic, InverseInPlace3x3)
{
    M33 a = {1, 2, 3, 0, 1, 4, 5, 6, 0};
    a.inverseInPlace();
    EXPECT_TRUE(a.isApprox({-24, 18, 5, 20, -15, -4, -5, 4, 1}, 1e-4f));
}

TEST(MatrixArithmetic, InverseInPlace1x1)
{
    M11 a = {5};
    a.inverseInPlace();
    M11 expected = {0.2f};
    EXPECT_TRUE(a.isApprox(expected, 1e-5f));
}

TEST(MatrixArithmetic, InverseInPlaceIdentity)
{
    M22 a(SpecialMatrixTag::E);
    a.inverseInPlace();
    M22 eye(SpecialMatrixTag::E);
    EXPECT_TRUE(a.isApprox(eye, 1e-5f));
}

TEST(MatrixArithmetic, TransposeInPlace)
{
    M22 a = {1, 2, 3, 4};
    a.transposeInPlace();
    M22 expected = {1, 3, 2, 4};
    EXPECT_EQ(a, expected);
}

// ============================================================
// Operator Immutability (originals unchanged after binary ops)
// ============================================================
TEST(MatrixImmutability, Add)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    M22 a_saved = a;
    M22 b_saved = b;
    static_cast<void>(a + b);
    EXPECT_EQ(a, a_saved);
    EXPECT_EQ(b, b_saved);
}

TEST(MatrixImmutability, Sub)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    M22 a_saved = a;
    M22 b_saved = b;
    static_cast<void>(a - b);
    EXPECT_EQ(a, a_saved);
    EXPECT_EQ(b, b_saved);
}

TEST(MatrixImmutability, ScalarMul)
{
    M22 a = {1, 2, 3, 4};
    M22 a_saved = a;
    static_cast<void>(a * 2.0f);
    EXPECT_EQ(a, a_saved);
}

TEST(MatrixImmutability, ScalarMulLeft)
{
    M22 a = {1, 2, 3, 4};
    M22 a_saved = a;
    static_cast<void>(2.0f * a);
    EXPECT_EQ(a, a_saved);
}

TEST(MatrixImmutability, MatMul)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    M22 a_saved = a;
    M22 b_saved = b;
    static_cast<void>(a * b);
    EXPECT_EQ(a, a_saved);
    EXPECT_EQ(b, b_saved);
}

TEST(MatrixImmutability, MatMul2x3x3x2)
{
    M23 a = {1, 2, 3, 4, 5, 6};
    M32 b = {1, 2, 3, 4, 5, 6};
    M23 a_saved = a;
    M32 b_saved = b;
    static_cast<void>(a * b);
    EXPECT_EQ(a, a_saved);
    EXPECT_EQ(b, b_saved);
}

TEST(MatrixImmutability, ScalarDiv)
{
    M22 a = {2, 4, 6, 8};
    M22 a_saved = a;
    static_cast<void>(a / 2.0f);
    EXPECT_EQ(a, a_saved);
}

TEST(MatrixImmutability, ScalarDivLeft)
{
    M22 a = {1, 2, 3, 4};
    M22 a_saved = a;
    static_cast<void>(2.0f / a);
    EXPECT_EQ(a, a_saved);
}

TEST(MatrixImmutability, MatDiv)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    M22 a_saved = a;
    M22 b_saved = b;
    static_cast<void>(a / b);
    EXPECT_EQ(a, a_saved);
    EXPECT_EQ(b, b_saved);
}

TEST(MatrixImmutability, TransposeSquare)
{
    M22 a = {1, 2, 3, 4};
    M22 a_saved = a;
    static_cast<void>(a.transpose());
    EXPECT_EQ(a, a_saved);
}

TEST(MatrixImmutability, TransposeNonSquare)
{
    M23 a = {1, 2, 3, 4, 5, 6};
    M23 a_saved = a;
    static_cast<void>(a.transpose());
    EXPECT_EQ(a, a_saved);
}

TEST(MatrixImmutability, Inverse)
{
    M22 a = {1, 2, 3, 4};
    M22 a_saved = a;
    static_cast<void>(a.inverse());
    EXPECT_EQ(a, a_saved);
}

TEST(MatrixImmutability, Inverse3x3)
{
    M33 a = {1, 2, 3, 0, 1, 4, 5, 6, 0};
    M33 a_saved = a;
    static_cast<void>(a.inverse());
    EXPECT_EQ(a, a_saved);
}

// ============================================================
// Compound Assignment (modifies *this)
// ============================================================
TEST(MatrixCompound, AddEq)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    a += b;
    M22 expected = {6, 8, 10, 12};
    EXPECT_EQ(a, expected);
}

TEST(MatrixCompound, SubEq)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    a -= b;
    M22 expected = {-4, -4, -4, -4};
    EXPECT_EQ(a, expected);
}

TEST(MatrixCompound, ScalarMulEq)
{
    M22 a = {1, 2, 3, 4};
    a *= 2.0f;
    M22 expected = {2, 4, 6, 8};
    EXPECT_EQ(a, expected);
}

TEST(MatrixCompound, MatMulEq)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    a *= b;
    M22 expected = {19, 22, 43, 50};
    EXPECT_EQ(a, expected);
}

TEST(MatrixCompound, ScalarDivEq)
{
    M22 a = {2, 4, 6, 8};
    a /= 2.0f;
    M22 expected = {1, 2, 3, 4};
    EXPECT_EQ(a, expected);
}

TEST(MatrixCompound, InverseInPlace)
{
    M22 a = {1, 2, 3, 4};
    a.inverseInPlace();
    M22 expected = {-2.0f, 1.0f, 1.5f, -0.5f};
    EXPECT_TRUE(a.isApprox(expected, 1e-5f));
}

TEST(MatrixCompound, MatDivEq)
{
    M22 a = {1, 2, 3, 4};
    M22 b = {5, 6, 7, 8};
    a /= b;
    M22 expected = {3, -2, 2, -1};
    EXPECT_TRUE(a.isApprox(expected, 1e-5f));
}

// ============================================================
// Cross-dimension / Identity Operations
// ============================================================
TEST(MatrixCrossDim, IdentityMulMatrix)
{
    M22 eye(SpecialMatrixTag::E);
    M22 a = {1, 2, 3, 4};
    EXPECT_EQ(eye * a, a);
}

TEST(MatrixCrossDim, MatrixMulIdentity)
{
    M22 eye(SpecialMatrixTag::E);
    M22 a = {1, 2, 3, 4};
    EXPECT_EQ(a * eye, a);
}

// ============================================================
// Edge Cases
// ============================================================
TEST(MatrixEdge, OneByOne)
{
    M11 a = {5};
    M11 b = {3};
    EXPECT_FLOAT_EQ((a + b)(0, 0), 8.0f);
    EXPECT_FLOAT_EQ((a - b)(0, 0), 2.0f);
    EXPECT_FLOAT_EQ((a * 2.0f)(0, 0), 10.0f);
    EXPECT_FLOAT_EQ((2.0f * a)(0, 0), 10.0f);
    EXPECT_FLOAT_EQ((a * b)(0, 0), 15.0f);
    EXPECT_FLOAT_EQ((a / 2.0f)(0, 0), 2.5f);
    EXPECT_FLOAT_EQ(a.inverse()(0, 0), 0.2f);
}

TEST(MatrixEdge, ZeroMatrixMul)
{
    M22 zero;
    M22 a = {1, 2, 3, 4};
    EXPECT_EQ(zero * a, zero);
    EXPECT_EQ(a * zero, zero);
}

// ============================================================
// Determinant
// ============================================================

// --- Non-square ---

TEST(MatrixDeterminant, NonSquareReturnsZero)
{
    M23 m = {1, 2, 3, 4, 5, 6};
    EXPECT_FLOAT_EQ(m.determinant(), 0.0f);
}

// --- 1x1 ---

TEST(MatrixDeterminant, OneByOne)
{
    M11 m = {5};
    EXPECT_FLOAT_EQ(m.determinant(), 5.0f);
}

// --- 2x2 ---

TEST(MatrixDeterminant, TwoByTwo)
{
    M22 m = {1, 2, 3, 4};
    EXPECT_FLOAT_EQ(m.determinant(), -2.0f);
}

TEST(MatrixDeterminant, TwoByTwoIdentity)
{
    M22 m(SpecialMatrixTag::E);
    EXPECT_FLOAT_EQ(m.determinant(), 1.0f);
}

TEST(MatrixDeterminant, TwoByTwoSingular)
{
    M22 m = {1, 2, 2, 4};
    EXPECT_FLOAT_EQ(m.determinant(), 0.0f);
}

// --- 3x3 ---

TEST(MatrixDeterminant, ThreeByThree)
{
    M33 m = {1, 2, 3, 0, 1, 4, 5, 6, 0};
    EXPECT_FLOAT_EQ(m.determinant(), 1.0f);
}

TEST(MatrixDeterminant, ThreeByThreeIdentity)
{
    M33 m(SpecialMatrixTag::E);
    EXPECT_FLOAT_EQ(m.determinant(), 1.0f);
}

TEST(MatrixDeterminant, ThreeByThreeSingular)
{
    M33 m = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_FLOAT_EQ(m.determinant(), 0.0f);
}

// --- 4x4 ---

TEST(MatrixDeterminant, FourByFourIdentity)
{
    M44 m(SpecialMatrixTag::E);
    EXPECT_FLOAT_EQ(m.determinant(), 1.0f);
}

TEST(MatrixDeterminant, FourByFourDiagonal)
{
    M44 m = {2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 5, 0, 0, 0, 0, 7};
    EXPECT_FLOAT_EQ(m.determinant(), 210.0f);
}

TEST(MatrixDeterminant, FourByFourSingular)
{
    M44 m = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_NEAR(m.determinant(), 0.0f, 1e-4f);
}

// --- 5x5 (PLU decomposition, >=5 path) ---

TEST(MatrixDeterminant, FiveByFiveIdentity)
{
    M55d m(SpecialMatrixTag::E);
    EXPECT_DOUBLE_EQ(m.determinant(), 1.0);
}

TEST(MatrixDeterminant, FiveByFiveDiagonal)
{
    M55d m = {2, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 6};
    EXPECT_DOUBLE_EQ(m.determinant(), 720.0);
}

TEST(MatrixDeterminant, FiveByFiveUpperTriangular)
{
    // clang-format off
    M55d m = {1, 1, 1, 1, 1,
              0, 2, 1, 1, 1,
              0, 0, 3, 1, 1,
              0, 0, 0, 4, 1,
              0, 0, 0, 0, 5};
    // clang-format on
    EXPECT_DOUBLE_EQ(m.determinant(), 120.0);
}

TEST(MatrixDeterminant, FiveByFiveSingularDuplicateRows)
{
    // clang-format off
    M55d m = {1, 2, 3, 4, 5,
              6, 7, 8, 9, 10,
              1, 2, 3, 4, 5,
              11,12,13,14,15,
              16,17,18,19,20};
    // clang-format on
    EXPECT_DOUBLE_EQ(m.determinant(), 0.0);
}

// --- 6x6 ---

TEST(MatrixDeterminant, SixBySixIdentity)
{
    M66d m(SpecialMatrixTag::E);
    EXPECT_DOUBLE_EQ(m.determinant(), 1.0);
}

TEST(MatrixDeterminant, SixBySixDiagonal)
{
    // clang-format off
    M66d m = {2, 0, 0, 0, 0, 0,
              0, 3, 0, 0, 0, 0,
              0, 0, 4, 0, 0, 0,
              0, 0, 0, 5, 0, 0,
              0, 0, 0, 0, 6, 0,
              0, 0, 0, 0, 0, 7};
    // clang-format on
    EXPECT_DOUBLE_EQ(m.determinant(), 5040.0);
}

// --- Integer determinant via free function (Bareiss algorithm) ---

TEST(MatrixDeterminant, IntegerBareiss5x5)
{
    const int data[25] = {2, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 6};
    EXPECT_EQ(calculateGenericMatrixDeterminant(std::span<const int>{data, 25}, 5, 5), 720);
}

TEST(MatrixDeterminant, IntegerBareissSingular5x5)
{
    // clang-format off
    const int data[25] = {1, 2, 3, 4, 5,
                          6, 7, 8, 9, 10,
                          1, 2, 3, 4, 5,
                          11,12,13,14,15,
                          16,17,18,19,20};
    // clang-format on
    EXPECT_EQ(calculateGenericMatrixDeterminant(std::span<const int>{data, 25}, 5, 5), 0);
}

TEST(MatrixDeterminant, IntegerBareissUpperTriangular5x5)
{
    // clang-format off
    const int data[25] = {1, 1, 1, 1, 1,
                          0, 2, 1, 1, 1,
                          0, 0, 3, 1, 1,
                          0, 0, 0, 4, 1,
                          0, 0, 0, 0, 5};
    // clang-format on
    EXPECT_EQ(calculateGenericMatrixDeterminant(std::span<const int>{data, 25}, 5, 5), 120);
}
