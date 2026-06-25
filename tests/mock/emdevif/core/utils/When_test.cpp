#include "host_test_pch.hpp"
#include <string_view>
#if EMDEVIF_USE_MODULES
import emdevif.core.utils.when;
#else
#include "emdevif/core/utils/when.hpp"
#endif
using namespace emdevif;

TEST(WhenTest, BasicMatching)
{
    int r = emdevif::when(2, 1, [] { return -1; }, 2, [] { return -2; });
    EXPECT_EQ(r, -2);
}
TEST(WhenTest, NoMatchReturnsDefault)
{
    int r = emdevif::when(99, 1, [] { return -1; }, 2, [] { return -2; });
    EXPECT_EQ(r, 0);
}
TEST(WhenTest, DefaultTag)
{
    int r = emdevif::when(99, 1, [] { return -1; }, default_tag, [] { return 999; });
    EXPECT_EQ(r, 999);
}
TEST(WhenTest, VoidReturn)
{
    int se = 0;
    emdevif::when(2, 1, [&] { se = 1; }, 2, [&] { se = 2; });
    EXPECT_EQ(se, 2);
}
TEST(WhenTest, StringView)
{
    std::string_view s = "world";
    int r = emdevif::when(s, "hello", [] { return 1; }, "world", [] { return 2; });
    EXPECT_EQ(r, 2);
}
TEST(WhenTest, Constexpr)
{
    constexpr int r = emdevif::when(2, 1, [] { return -1; }, 2, [] { return -2; });
    static_assert(r == -2);
}
