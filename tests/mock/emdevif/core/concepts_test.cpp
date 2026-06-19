#include "host_test_pch.hpp"
#include <type_traits>
#include <concepts>
#include <compare>
#include <limits>
#if EMDEVIF_USE_MODULES
    import emdevif.core.concepts;
#else
    #include "emdevif/core/concepts.hpp"
#endif
using namespace emdevif;

class CustomArith {
public:
    int v = 0;
    static constexpr int max() { return 100; }
    static constexpr int min() { return 0; }
    CustomArith operator+(CustomArith o) const { return {v+o.v}; }
    CustomArith operator-(CustomArith o) const { return {v-o.v}; }
    CustomArith operator*(CustomArith o) const { return {v*o.v}; }
    CustomArith operator/(CustomArith o) const { return {v/o.v}; }
    auto operator<=>(const CustomArith&) const = default;
};
class NoLimits {};

TEST(ConceptsTest, HaveLimitType) {
    static_assert(HaveLimitType<int>);
    static_assert(HaveLimitType<float>);
    static_assert(HaveLimitType<CustomArith>);
    static_assert(!HaveLimitType<NoLimits>);
}
TEST(ConceptsTest, ArithmeticType) {
    static_assert(ArithmeticType<int>);
    static_assert(ArithmeticType<float>);
    static_assert(ArithmeticType<CustomArith>);
}
TEST(ConceptsTest, FloatingPointType) {
    static_assert(FloatingPointType<float>);
    static_assert(FloatingPointType<double>);
    static_assert(!FloatingPointType<int>);
}
TEST(ConceptsTest, PointerType) {
    static_assert(PointerType<int*>);
    static_assert(PointerType<const char*>);
    static_assert(!PointerType<int>);
}
