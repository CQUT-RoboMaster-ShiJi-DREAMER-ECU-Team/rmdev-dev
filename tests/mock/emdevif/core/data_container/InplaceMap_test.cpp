#include "host_test_pch.hpp"
#include <string>
#include <string_view>

#if EMDEVIF_USE_MODULES
import emdevif.core.data_container.inplace_map;
import emdevif.core.error_handler;
#else
#include "emdevif/core/data_container/inplace_map.hpp"
#endif

using namespace emdevif;

TEST(InplaceMapTest, DefaultState)
{
    InplaceMap<int, std::string, 4> map;
    EXPECT_EQ(map.size(), 0u);
    EXPECT_TRUE(map.isEmpty());
    EXPECT_FALSE(map.isFull());
    EXPECT_EQ(map.capacity(), 4u);
    EXPECT_EQ(map.maxSize(), 4u);
}

TEST(InplaceMapTest, InsertAndAccess)
{
    InplaceMap<int, std::string, 4> map;
    auto [it1, ec1] = map.insert({1, "one"});
    EXPECT_EQ(ec1, ErrorCode::Success);
    ASSERT_NE(it1, map.end());
    EXPECT_EQ(it1->first, 1);
    EXPECT_EQ(it1->second, "one");

    auto [it2, ec2] = map.insert({2, "two"});
    EXPECT_EQ(ec2, ErrorCode::Success);

    auto [it3, ec3] = map.insert({1, "ONE"});
    EXPECT_EQ(ec3, ErrorCode::AlreadyExists);
    EXPECT_EQ(it3->second, "one");

    EXPECT_EQ(map.size(), 2u);
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
    EXPECT_EQ(map.at(1), &map[1]);
    EXPECT_EQ(map.at(3), nullptr);
}

TEST(InplaceMapTest, InsertWhenFull)
{
    InplaceMap<int, int, 2> map;
    EXPECT_EQ(map.insert({1, 10}).second, ErrorCode::Success);
    EXPECT_EQ(map.insert({2, 20}).second, ErrorCode::Success);
    EXPECT_EQ(map.insert({3, 30}).second, ErrorCode::Full);
    EXPECT_TRUE(map.isFull());
}

TEST(InplaceMapTest, EmplaceAndTryEmplace)
{
    InplaceMap<int, std::string, 4> map;
    auto [it1, inserted1] = map.tryEmplace(1, 3, 'a');
    EXPECT_TRUE(inserted1);
    EXPECT_EQ(it1->second, "aaa");

    auto [it2, inserted2] = map.tryEmplace(1, "bbb");
    EXPECT_FALSE(inserted2);
    EXPECT_EQ(it2->second, "aaa");

    auto [it3, ec3] = map.emplace(2, 4, 'b');
    EXPECT_EQ(ec3, ErrorCode::Success);
    EXPECT_EQ(it3->second, "bbbb");
}

TEST(InplaceMapTest, InsertOrAssign)
{
    InplaceMap<int, std::string, 4> map;
    map.insert({1, "one"});
    auto [it1, ec1] = map.insertOrAssign(1, std::string{"ONE"});
    EXPECT_EQ(ec1, ErrorCode::AlreadyExists);
    EXPECT_EQ(map[1], "ONE");

    auto [it2, ec2] = map.insertOrAssign(2, std::string{"two"});
    EXPECT_EQ(ec2, ErrorCode::Success);
    EXPECT_EQ(map[2], "two");
}

TEST(InplaceMapTest, GetOrCreate)
{
    InplaceMap<int, std::string, 4> map;
    auto* p1 = map.getOrCreate(1);
    ASSERT_NE(p1, nullptr);
    EXPECT_TRUE(p1->empty());
    *p1 = "one";

    auto* p2 = map.getOrCreate(1);
    EXPECT_EQ(p2, p1);
    EXPECT_EQ(*p2, "one");

    EXPECT_NE(map.getOrCreate(2), nullptr);
    EXPECT_NE(map.getOrCreate(3), nullptr);
    EXPECT_NE(map.getOrCreate(4), nullptr);
    EXPECT_EQ(map.getOrCreate(5), nullptr);
}

TEST(InplaceMapTest, FindAndContains)
{
    InplaceMap<int, int, 4> map;
    map.insert({1, 10});
    map.insert({3, 30});
    map.insert({5, 50});

    EXPECT_TRUE(map.contains(3));
    EXPECT_FALSE(map.contains(2));
    EXPECT_EQ(map.count(1), 1u);
    EXPECT_EQ(map.count(2), 0u);

    auto it = map.find(3);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->second, 30);
    EXPECT_EQ(map.find(2), map.end());
}

TEST(InplaceMapTest, Bounds)
{
    InplaceMap<int, int, 5> map;
    map.insert({1, 10});
    map.insert({3, 30});
    map.insert({5, 50});

    auto lb = map.lowerBound(3);
    ASSERT_NE(lb, map.end());
    EXPECT_EQ(lb->first, 3);

    auto ub = map.upperBound(3);
    ASSERT_NE(ub, map.end());
    EXPECT_EQ(ub->first, 5);

    auto [l, u] = map.equalRange(3);
    EXPECT_EQ(l->first, 3);
    EXPECT_EQ(u->first, 5);
}

TEST(InplaceMapTest, Erase)
{
    InplaceMap<int, int, 5> map;
    map.insert({1, 10});
    map.insert({2, 20});
    map.insert({3, 30});

    EXPECT_EQ(map.erase(2), 1u);
    EXPECT_EQ(map.size(), 2u);
    EXPECT_FALSE(map.contains(2));

    EXPECT_EQ(map.erase(9), 0u);

    auto it = map.find(3);
    auto next = map.erase(it);
    EXPECT_EQ(next, map.end());
    EXPECT_EQ(map.size(), 1u);
}

TEST(InplaceMapTest, Clear)
{
    InplaceMap<int, int, 4> map;
    map.insert({1, 10});
    map.insert({2, 20});
    map.clear();
    EXPECT_TRUE(map.isEmpty());
    EXPECT_EQ(map.find(1), map.end());
}

TEST(InplaceMapTest, Comparisons)
{
    InplaceMap<int, int, 4> a;
    a.insert({1, 10});
    a.insert({2, 20});
    InplaceMap<int, int, 4> b;
    b.insert({1, 10});
    b.insert({3, 30});
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(a == b);

    InplaceMap<int, int, 4> c;
    c.insert({1, 10});
    c.insert({2, 20});
    EXPECT_TRUE(a == c);
}

TEST(InplaceMapTest, Iteration)
{
    InplaceMap<int, int, 5> map;
    map.insert({3, 30});
    map.insert({1, 10});
    map.insert({2, 20});

    int expected = 1;
    for (const auto& [k, v] : map) {
        EXPECT_EQ(k, expected);
        EXPECT_EQ(v, expected * 10);
        ++expected;
    }
    EXPECT_EQ(expected, 4);
}

TEST(InplaceMapTest, CustomComparator)
{
    InplaceMap<int, int, 4, std::greater<int>> map;
    map.insert({1, 10});
    map.insert({3, 30});
    map.insert({2, 20});

    int expected = 3;
    for (const auto& [k, v] : map) {
        EXPECT_EQ(k, expected);
        --expected;
    }
}

TEST(InplaceMapTest, RangeInsert)
{
    InplaceMap<int, int, 5> map;
    std::array<std::pair<int, int>, 3> vals{{{3, 30}, {1, 10}, {2, 20}}};
    map.insert(vals.begin(), vals.end());
    EXPECT_EQ(map.size(), 3u);
    EXPECT_EQ(map[1], 10);
    EXPECT_EQ(map[2], 20);
    EXPECT_EQ(map[3], 30);
}

TEST(InplaceMapTest, InitializerList)
{
    InplaceMap<int, std::string, 4> map{{{2, "two"}, {1, "one"}}};
    EXPECT_EQ(map.size(), 2u);
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
}

TEST(InplaceMapTest, MoveOnlyMappedType)
{
    struct MoveOnly {
        int value;
        explicit MoveOnly(int v) : value(v) {}
        MoveOnly(const MoveOnly&) = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;
        MoveOnly(MoveOnly&&) = default;
        MoveOnly& operator=(MoveOnly&&) = default;
    };

    InplaceMap<int, MoveOnly, 3> map;
    auto [it, ec] = map.emplace(1, 42);
    EXPECT_EQ(ec, ErrorCode::Success);
    EXPECT_EQ(it->second.value, 42);
}
