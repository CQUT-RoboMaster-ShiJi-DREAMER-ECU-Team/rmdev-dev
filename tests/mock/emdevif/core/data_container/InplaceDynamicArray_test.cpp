#include "host_test_pch.hpp"
#include <algorithm>
#include <array>
#include <string>

#if EMDEVIF_USE_MODULES
import emdevif.core.data_container.inplace_dynamic_array;
import emdevif.core.error_handler;
#else
#include "emdevif/core/data_container/inplace_dynamic_array.hpp"
#endif

using namespace emdevif;

TEST(InplaceDynamicArrayTest, DefaultState)
{
    InplaceDynamicArray<int, 5> arr;
    EXPECT_EQ(arr.size(), 0u);
    EXPECT_TRUE(arr.isEmpty());
    EXPECT_FALSE(arr.isFull());
    EXPECT_EQ(arr.capacity(), 5u);
    EXPECT_EQ(arr.maxSize(), 5u);
}

TEST(InplaceDynamicArrayTest, PushBackAndAccess)
{
    InplaceDynamicArray<int, 5> arr;
    EXPECT_EQ(arr.pushBack(1), ErrorCode::Success);
    EXPECT_EQ(arr.pushBack(2), ErrorCode::Success);
    EXPECT_EQ(arr.pushBack(3), ErrorCode::Success);
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr.front(), 1);
    EXPECT_EQ(arr.back(), 3);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(*arr.at(1), 2);
    EXPECT_EQ(arr.at(5), nullptr);
}

TEST(InplaceDynamicArrayTest, PushBackWhenFull)
{
    InplaceDynamicArray<int, 2> arr;
    EXPECT_EQ(arr.pushBack(1), ErrorCode::Success);
    EXPECT_EQ(arr.pushBack(2), ErrorCode::Success);
    EXPECT_EQ(arr.pushBack(3), ErrorCode::Full);
    EXPECT_TRUE(arr.isFull());
}

TEST(InplaceDynamicArrayTest, TryPushBack)
{
    InplaceDynamicArray<int, 2> arr;
    EXPECT_NE(arr.tryPushBack(1), nullptr);
    EXPECT_NE(arr.tryPushBack(2), nullptr);
    EXPECT_EQ(arr.tryPushBack(3), nullptr);
}

TEST(InplaceDynamicArrayTest, EmplaceBack)
{
    InplaceDynamicArray<std::pair<int, int>, 3> arr;
    EXPECT_EQ(arr.emplaceBack(1, 2), ErrorCode::Success);
    EXPECT_EQ(arr.emplaceBack(3, 4), ErrorCode::Success);
    EXPECT_EQ(arr[0].first, 1);
    EXPECT_EQ(arr[1].second, 4);
    EXPECT_EQ(arr.emplaceBack(5, 6), ErrorCode::Success);
    EXPECT_EQ(arr.emplaceBack(7, 8), ErrorCode::Full);
}

TEST(InplaceDynamicArrayTest, PopBack)
{
    InplaceDynamicArray<int, 5> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.popBack();
    EXPECT_EQ(arr.size(), 1u);
    EXPECT_EQ(arr.back(), 1);
    arr.popBack();
    EXPECT_TRUE(arr.isEmpty());
    arr.popBack();  // no effect
    EXPECT_TRUE(arr.isEmpty());
}

TEST(InplaceDynamicArrayTest, InsertSingle)
{
    InplaceDynamicArray<int, 5> arr;
    arr.pushBack(1);
    arr.pushBack(3);
    auto [it, ec] = arr.insert(arr.cbegin() + 1, 2);
    EXPECT_EQ(ec, ErrorCode::Success);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(InplaceDynamicArrayTest, InsertCount)
{
    InplaceDynamicArray<int, 6> arr;
    arr.pushBack(1);
    arr.pushBack(5);
    auto [it, ec] = arr.insert(arr.cbegin() + 1, 3, 9);
    EXPECT_EQ(ec, ErrorCode::Success);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(arr.size(), 5u);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 9);
    EXPECT_EQ(arr[2], 9);
    EXPECT_EQ(arr[3], 9);
    EXPECT_EQ(arr[4], 5);
}

TEST(InplaceDynamicArrayTest, InsertRange)
{
    InplaceDynamicArray<int, 6> arr;
    arr.pushBack(1);
    arr.pushBack(6);
    std::array<int, 3> vals{2, 3, 4};
    auto [it, ec] = arr.insert(arr.cbegin() + 1, vals.begin(), vals.end());
    EXPECT_EQ(ec, ErrorCode::Success);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(arr.size(), 5u);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[3], 4);
    EXPECT_EQ(arr[4], 6);
}

TEST(InplaceDynamicArrayTest, InsertWhenFull)
{
    InplaceDynamicArray<int, 3> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    auto [it, ec] = arr.insert(arr.cbegin(), 9);
    EXPECT_EQ(ec, ErrorCode::Full);
    EXPECT_EQ(it, nullptr);
}

TEST(InplaceDynamicArrayTest, Erase)
{
    InplaceDynamicArray<int, 5> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    arr.pushBack(4);
    auto it = arr.erase(arr.cbegin() + 1);
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 3);
    EXPECT_EQ(arr[2], 4);
}

TEST(InplaceDynamicArrayTest, EraseRange)
{
    InplaceDynamicArray<int, 5> arr;
    for (int i = 1; i <= 5; ++i) {
        arr.pushBack(i);
    }
    auto it = arr.erase(arr.cbegin() + 1, arr.cbegin() + 3);
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_EQ(*it, 4);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 4);
    EXPECT_EQ(arr[2], 5);
}

TEST(InplaceDynamicArrayTest, Assign)
{
    InplaceDynamicArray<int, 5> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    EXPECT_EQ(arr.assign(4, 7), ErrorCode::Success);
    EXPECT_EQ(arr.size(), 4u);
    for (std::size_t i = 0; i < arr.size(); ++i) {
        EXPECT_EQ(arr[i], 7);
    }
    EXPECT_EQ(arr.assign(6, 7), ErrorCode::InvalidArgument);
}

TEST(InplaceDynamicArrayTest, Resize)
{
    InplaceDynamicArray<int, 5> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    EXPECT_EQ(arr.resize(4), ErrorCode::Success);
    EXPECT_EQ(arr.size(), 4u);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    arr.resize(1);
    EXPECT_EQ(arr.size(), 1u);
    EXPECT_EQ(arr.resize(6), ErrorCode::InvalidArgument);
}

TEST(InplaceDynamicArrayTest, Clear)
{
    InplaceDynamicArray<int, 5> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.clear();
    EXPECT_TRUE(arr.isEmpty());
}

TEST(InplaceDynamicArrayTest, Swap)
{
    InplaceDynamicArray<int, 5> a;
    a.pushBack(1);
    a.pushBack(2);
    InplaceDynamicArray<int, 5> b;
    b.pushBack(3);
    a.swap(b);
    EXPECT_EQ(a.size(), 1u);
    EXPECT_EQ(a[0], 3);
    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
}

TEST(InplaceDynamicArrayTest, Comparisons)
{
    InplaceDynamicArray<int, 5> a{1, 2, 3};
    InplaceDynamicArray<int, 5> b{1, 2, 4};
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(a == b);
    InplaceDynamicArray<int, 5> c{1, 2, 3};
    EXPECT_TRUE(a == c);
}

TEST(InplaceDynamicArrayTest, NonMemberErase)
{
    InplaceDynamicArray<int, 5> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    arr.pushBack(2);
    EXPECT_EQ(erase(arr, 2), 2u);
    EXPECT_EQ(arr.size(), 2u);
}

TEST(InplaceDynamicArrayTest, InitializerList)
{
    InplaceDynamicArray<int, 5> arr{1, 2, 3};
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr[2], 3);
}

TEST(InplaceDynamicArrayTest, MoveOnlyType)
{
    struct MoveOnly {
        int value;
        explicit MoveOnly(int v) : value(v) {}
        MoveOnly(const MoveOnly&) = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;
        MoveOnly(MoveOnly&&) = default;
        MoveOnly& operator=(MoveOnly&&) = default;
    };

    InplaceDynamicArray<MoveOnly, 3> arr;
    EXPECT_EQ(arr.emplaceBack(1), ErrorCode::Success);
    EXPECT_EQ(arr.emplaceBack(2), ErrorCode::Success);
    EXPECT_NE(arr.tryEmplaceBack(3), nullptr);
}
