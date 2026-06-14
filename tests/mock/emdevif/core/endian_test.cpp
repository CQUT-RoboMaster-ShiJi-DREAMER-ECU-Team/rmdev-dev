#include "host_test_pch.hpp"
#include <cstdint>
#include "emdevif/core/endian.hpp"
using namespace emdevif;

TEST(EndianTest, ByteSwapUint16) {
    EXPECT_EQ(byteSwap(uint16_t{0x1234}), uint16_t{0x3412});
}
TEST(EndianTest, ByteSwapUint32) {
    EXPECT_EQ(byteSwap(uint32_t{0x12345678}), uint32_t{0x78563412});
}
TEST(EndianTest, DoubleSwapRestores) {
    uint32_t v = 0xDEADBEEF;
    EXPECT_EQ(byteSwap(byteSwap(v)), v);
    float f = 3.14f;
    EXPECT_EQ(byteSwap(byteSwap(f)), f);
}
TEST(EndianTest, ConstexprByteSwap) {
    constexpr uint16_t v = 0xABCD;
    constexpr uint16_t r = byteSwap(v);
    static_assert(r == 0xCDAB);
}
