#include "host_test_pch.hpp"
#include <cstring>
#include <array>
#include <string_view>

#if EMDEVIF_USE_MODULES
import emdevif.core.data_container.array_map;
#else
#include "emdevif/core/data_container/array_map.hpp"
#endif
#if EMDEVIF_USE_MODULES
import emdevif.core.integer_suffix;
#else
#include "emdevif/core/integer_suffix.hpp"
#endif

using namespace emdevif;

namespace {
static constexpr auto map1 = emdevif::makeStaticMap<int, char>({{1, 'a'}, {2, 'b'}, {114514, 'k'}});
static_assert(map1.find(1919810) == map1.cend());
static_assert(map1.at(9) == nullptr);
static_assert(*map1.at(2) == 'b');
static_assert(map1[2] == 'b');
static_assert(map1[114514] == 'k');
static_assert(map1.maxSize() == map1.size());
static_assert(map1.size() == 3);
}  // namespace

TEST(ArrayMapTest, ConstructTest)
{
    ArrayMap<std::string_view, int, 4> map1{{{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}}};
    EXPECT_EQ(map1.size(), 4u);
    EXPECT_EQ(map1.maxSize(), 4u);
    EXPECT_EQ(map1["three"], 3);
    EXPECT_EQ(map1["four"], 4);
    EXPECT_TRUE(map1.isFull());
    EXPECT_FALSE(map1.isEmpty());
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
    map1["three"] = 333;
    map1["four"] = 444;
    EXPECT_EQ(map1["three"], 333);
    EXPECT_EQ(map1["four"], 444);
    auto map3 = makeArrayMap<std::string_view, int, 10>({{"qwe", 23}, {"rty", 12}});
    EXPECT_EQ(map3.size(), 2u);
    EXPECT_EQ(map3.maxSize(), 10u);
}

TEST(ArrayMapTest, InsertEraseTest)
{
    auto map = makeArrayMap<std::string_view, int, 10>({{"qwe", 23}, {"rty", 12}});
    EXPECT_EQ(map.size(), 2u);
    map.insert({"three", 3});
    EXPECT_EQ(map.size(), 3u);
    EXPECT_EQ(map["three"], 3);
    auto ret = map.erase("rty");
    EXPECT_EQ(1u, ret);
    EXPECT_TRUE(map.find("rty") == map.end());
    ret = map.erase("114514");
    EXPECT_EQ(0u, ret);
}

TEST(ArrayMapTest, EraseIfTest)
{
    auto map = makeArrayMap<int, char>({{1, 'a'}, {2, 'b'}, {3, 'c'}, {4, 'd'}, {5, 'e'}});
    auto count = erase_if(map, [](const auto& item) {
        auto const& [key, value] = item;
        return (key % 2) != 0;
    });
    EXPECT_EQ(count, 3u);
    EXPECT_EQ(map.size(), 2u);
    constexpr auto expect_list = std::to_array<std::pair<int, char>>({{2, 'b'}, {4, 'd'}});
    auto idx = 0_zu;
    for (auto& e : map) {
        auto [k, v] = e;
        EXPECT_EQ(expect_list[idx].first, k);
        ++idx;
    }
}

TEST(ArrayMapTest, KeyEqualTParamTest)
{
    struct constCharPtrEq {
        bool operator()(char const* l, char const* r) const
        {
            return std::strcmp(l, r) == 0;
        }
    };
    auto map = makeArrayMap<const char*, int, 10, constCharPtrEq>({{"qwe", 23}, {"rty", 12}});
    EXPECT_EQ(map["qwe"], 23);
    map.insert({"three", 3});
    EXPECT_EQ(map.size(), 3u);
    auto ret = map.erase("rty");
    EXPECT_EQ(1u, ret);
    EXPECT_TRUE(map.find("rty") == map.end());
}
