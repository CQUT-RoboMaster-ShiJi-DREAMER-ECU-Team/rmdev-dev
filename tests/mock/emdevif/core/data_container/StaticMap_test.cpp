#include "host_test_pch.hpp"
#include <cstring>
#include <array>
#include <string_view>

#if EMDEVIF_USE_MODULES
import emdevif.core.data_container.static_map;
#else
#include "emdevif/core/data_container/static_map.hpp"
#endif

using namespace emdevif;

namespace {
static constexpr auto map1 = emdevif::StaticMap<int, char, 3>{{{1, 'a'}, {2, 'b'}, {114514, 'k'}}};
static_assert(map1.find(1919810) == map1.cend());
static_assert(map1.at(9) == nullptr);
static_assert(*map1.at(2) == 'b');
static_assert(map1[2] == 'b');
static_assert(map1[114514] == 'k');
static_assert(map1.maxSize() == map1.size());
static_assert(map1.size() == 3);
}  // namespace

TEST(StaticMapTest, ConstructTest)
{
    StaticMap<std::string_view, int, 4> map1{{{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}}};
    EXPECT_EQ(map1.size(), 4u);
    EXPECT_EQ(map1.maxSize(), 4u);
    EXPECT_EQ(map1["three"], 3);
    EXPECT_EQ(map1["four"], 4);
    for (int cnt = 1; auto& e : map1) {
        auto [k, v] = e;
        EXPECT_EQ(cnt, v);
        ++cnt;
    }
    decltype(map1) map2 = map1;
    for (int cnt = 1; auto& e : map2) {
        auto [k, v] = e;
        EXPECT_EQ(cnt, v);
        ++cnt;
    }
}

TEST(StaticMapTest, KeyEqualTParamTest)
{
    struct constCharPtrEq {
        constexpr bool operator()(char const* l, char const* r) const
        {
            while (*l != '\0' && *r != '\0' && *l == *r) {
                ++l;
                ++r;
            }
            return *l == *r;
        }
    };
    StaticMap<const char*, int, 4, constCharPtrEq> map1{{{"qwe", 23}, {"rty", 12}, {"three", 3}, {"four", 4}}};
    EXPECT_EQ(map1["qwe"], 23);
    EXPECT_EQ(map1["rty"], 12);
}
