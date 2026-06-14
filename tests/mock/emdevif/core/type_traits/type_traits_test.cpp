#include "host_test_pch.hpp"
#include <utility>
#include <tuple>
#include <array>
#include "emdevif/core/type_traits.hpp"
using namespace emdevif;

namespace { static constexpr auto a = 0; static auto b = 0;
static_assert(emdevif::is_consteval([] { return a; }));
static_assert(!emdevif::is_consteval([] { return b; })); }

namespace { struct S2 { int a; float b; };
using T2 = std::tuple<int, float>; using P2 = std::pair<int, float>;
static_assert(emdevif::is_tuple_like_v<T2>);
static_assert(emdevif::is_tuple_like_v<P2>);
static_assert(!emdevif::is_tuple_like_v<S2>);
static_assert(emdevif::is_tuple_style_v<T2>);
static_assert(emdevif::is_tuple_style_v<S2>);
static_assert(detail::is_std_array_v<std::array<int,3>>);
static_assert(!detail::is_std_array_v<int>); }

TEST(TypeTraitsTest, TupleToAggregate) {
    struct Point { int x; float y; };
    auto t = std::make_tuple(10, 3.14f);
    Point p = emdevif::tuple_to_aggregate<Point>(t);
    EXPECT_EQ(p.x, 10); EXPECT_FLOAT_EQ(p.y, 3.14f);
}
TEST(TypeTraitsTest, AggregateToTuple) {
    struct Point { int x; float y; };
    Point p{42, 2.71f};
    auto t = emdevif::aggregate_to_tuple(p);
    EXPECT_EQ(std::get<0>(t), 42);
    EXPECT_FLOAT_EQ(std::get<1>(t), 2.71f);
}
TEST(TypeTraitsTest, IsSameTupleStyle) {
    struct Point { int x; float y; };
    using T2 = std::tuple<int, float>;
    static_assert(emdevif::is_same_tuple_style_v<Point, T2>);
    struct Other { int a; double b; };
    static_assert(!emdevif::is_same_tuple_style_v<Point, Other>);
}
