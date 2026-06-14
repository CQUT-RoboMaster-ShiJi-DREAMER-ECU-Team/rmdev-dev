#include "host_test_pch.hpp"
#include <cstdint>
#include <type_traits>
#include "emdevif/core/integer_suffix.hpp"
using namespace emdevif::literals::integer_literals;

TEST(IntegerSuffixTest, Unsigned) {
    auto a = 42_u8; EXPECT_EQ(a, 42);
    static_assert(std::is_same_v<decltype(a), std::uint8_t>);
    auto b = 65535_u16; EXPECT_EQ(b, 65535);
    static_assert(std::is_same_v<decltype(b), std::uint16_t>);
}
TEST(IntegerSuffixTest, Signed) {
    auto a = 127_i8; EXPECT_EQ(a, 127);
    static_assert(std::is_same_v<decltype(a), std::int8_t>);
    auto b = 32767_i16; EXPECT_EQ(b, 32767);
    static_assert(std::is_same_v<decltype(b), std::int16_t>);
}
TEST(IntegerSuffixTest, SizeT) {
    auto a = 42_zu; EXPECT_EQ(a, 42u);
    static_assert(std::is_same_v<decltype(a), std::size_t>);
}
TEST(IntegerSuffixTest, Constexpr) {
    constexpr auto v = 42_u8; static_assert(v == 42);
}
