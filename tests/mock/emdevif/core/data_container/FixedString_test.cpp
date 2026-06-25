#include "host_test_pch.hpp"
#include <compare>
#include <string_view>
#if EMDEVIF_USE_MODULES
import emdevif.core.data_container.fixed_string;
#else
#include "emdevif/core/data_container/fixed_string.hpp"
#endif
using namespace emdevif;

TEST(FixedStringTest, CharPackConstruct)
{
    constexpr BasicFixedString<char, 3> s('a', 'b', 'c');
    EXPECT_EQ(s.view(), "abc");
}
TEST(FixedStringTest, CLiteralConstruct)
{
    constexpr BasicFixedString s = BasicFixedString<char, 3>("abc");
    EXPECT_EQ(s.view(), "abc");
}
TEST(FixedStringTest, IteratorConstruct)
{
    std::string_view src = "hello";
    BasicFixedString<char, 5> s(src.begin(), src.end());
    EXPECT_EQ(s.view(), "hello");
}
TEST(FixedStringTest, ElementAccess)
{
    constexpr BasicFixedString<char, 3> s('x', 'y', 'z');
    EXPECT_EQ(s[0], 'x');
    EXPECT_EQ(s[1], 'y');
    EXPECT_EQ(s[2], 'z');
    EXPECT_EQ(s.front(), 'x');
    EXPECT_EQ(s.back(), 'z');
}
TEST(FixedStringTest, Iterators)
{
    constexpr BasicFixedString<char, 4> s('a', 'b', 'c', 'd');
    std::string result;
    for (char c : s) {
        result += c;
    }
    EXPECT_EQ(result, "abcd");
}
TEST(FixedStringTest, ConcatStrings)
{
    constexpr BasicFixedString<char, 2> a('a', 'b');
    constexpr BasicFixedString<char, 3> b('c', 'd', 'e');
    constexpr auto c = a + b;
    static_assert(c.size() == 5);
    EXPECT_EQ(c.view(), "abcde");
}
TEST(FixedStringTest, ConcatChar)
{
    constexpr BasicFixedString<char, 2> a('a', 'b');
    constexpr auto b = a + 'c';
    EXPECT_EQ(b.view(), "abc");
    constexpr auto c = 'x' + a;
    EXPECT_EQ(c.view(), "xab");
}
TEST(FixedStringTest, Comparison)
{
    constexpr BasicFixedString<char, 3> a('a', 'b', 'c');
    constexpr BasicFixedString<char, 3> b('a', 'b', 'c');
    constexpr BasicFixedString<char, 3> c('a', 'b', 'd');
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a < c);
}
TEST(FixedStringTest, Swap)
{
    BasicFixedString<char, 3> a('f', 'o', 'o');
    BasicFixedString<char, 3> b('b', 'a', 'r');
    a.swap(b);
    EXPECT_EQ(a.view(), "bar");
    EXPECT_EQ(b.view(), "foo");
}
TEST(FixedStringTest, EmptyString)
{
    constexpr BasicFixedString<char, 0> s;
    static_assert(s.empty());
    static_assert(s.size() == 0);
}
