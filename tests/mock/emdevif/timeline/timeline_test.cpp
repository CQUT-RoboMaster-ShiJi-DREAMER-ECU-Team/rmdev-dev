#include "host_test_pch.hpp"
#include <cstdint>
#include "emdevif/timeline.hpp"
static std::uint64_t g_mt=0;
namespace emdevif::user_declares::timeline { std::uint64_t getMicroseconds() { return g_mt++; } }
using namespace emdevif;

TEST(TimelineTest, ConstAndUpdate) {
    g_mt=1000; Timeline t; t.update();
    EXPECT_EQ(t(),1000u);
}
TEST(TimelineTest, ConstructWithVal) {
    Timeline t(500); EXPECT_EQ(t(),500u);
}
TEST(TimelineTest, Assign) {
    Timeline t; t=100; EXPECT_EQ(t(),100u);
    g_mt=200; t.update(); EXPECT_EQ(t(),200u);
}
TEST(TimelineTest, ImplicitConvert) {
    Timeline t(42); uint64_t v=t; EXPECT_EQ(v,42u);
}
