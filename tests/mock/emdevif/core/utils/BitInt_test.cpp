#include "host_test_pch.hpp"
#include <cstdint>
#include <limits>
#if EMDEVIF_USE_MODULES
import emdevif.core.utils.bit_int;
#else
#include "emdevif/core/utils/bit_int.hpp"
#endif
using namespace emdevif;

TEST(BitIntTest, StdSizes)
{
    BitInt<8> a = int8_t{127};
    EXPECT_EQ(static_cast<int>(a), 127);
    a = int8_t{-128};
    EXPECT_EQ(static_cast<int8_t>(a), -128);
    BitInt<16> b{int16_t{32767}};
    EXPECT_EQ(b, int16_t{32767});
}
TEST(BitIntTest, NonStdSizes)
{
    static_assert(BitInt<4>::max() == 7);
    static_assert(BitInt<4>::min() == -8);
    BitInt<4> a = 5;
    EXPECT_EQ(static_cast<int8_t>(a), 5);
    a = -5;
    EXPECT_EQ(static_cast<int8_t>(a), -5);
    a = a.max();
    EXPECT_EQ(a, 7);
    a = a.min();
    EXPECT_EQ(a, -8);
    const BitInt<6> c1 = 10;
    BitInt<5> c2 = c1;
    EXPECT_EQ(c2, 10);
    c2 += 5;
    EXPECT_EQ(c2, 15);
}
TEST(BitIntTest, MixedArith)
{
    const BitInt<6> c1 = 10;
    BitInt<5> c2 = 10;
    c2 += 5;
    int t = c1 + c2 + 1;
    EXPECT_EQ(t, 26);
    BitInt<6> c6;
    c6 = 0b11101;
    EXPECT_EQ(c6 & 0b11111, 0b11101);
}
TEST(BitIntTest, UBitInt)
{
    emdevif::UBitInt<24> u24 = uint32_t{31};
    EXPECT_EQ(uint32_t{31}, u24);
}
