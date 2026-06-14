#include "host_test_pch.hpp"
#include <array>
#include <algorithm>
#include "emdevif/core/data_container/ring_buffer.hpp"
using namespace emdevif;

TEST(RingUnsignedTest, IncrementWrap) {
    RingUnsigned<size_t, 10> ru{0};
    EXPECT_EQ(static_cast<size_t>(ru), 0u);
    for (int i = 0; i < 9; ++i) ++ru;
    EXPECT_EQ(static_cast<size_t>(ru), 9u);
    ++ru; EXPECT_EQ(static_cast<size_t>(ru), 0u);
}
TEST(RingUnsignedTest, DecrementWrap) {
    RingUnsigned<size_t, 10> ru{0};
    --ru; EXPECT_EQ(static_cast<size_t>(ru), 9u);
}
TEST(RingUnsignedTest, SubtractClockwise) {
    RingUnsigned<size_t, 10> a{7}, b{2};
    EXPECT_EQ(static_cast<size_t>(a - b), 5u);
    RingUnsigned<size_t, 10> c{2}, d{8};
    EXPECT_EQ(static_cast<size_t>(c - d), 4u);
}

TEST(RingBufferTest, BasicReadWrite) {
    RingBuffer<char, 10> buffer;
    EXPECT_EQ(buffer.size(), 10u);
    EXPECT_EQ(buffer.usedSlots(), 0u);
    EXPECT_TRUE(buffer.isEmpty());
    buffer.push('A');
    EXPECT_EQ(buffer.usedSlots(), 1u);
    EXPECT_EQ(buffer.peek(), 'A');
    EXPECT_EQ(buffer.pop(), 'A');
    EXPECT_TRUE(buffer.isEmpty());
}
TEST(RingBufferTest, BatchReadWrite) {
    RingBuffer<char, 10> buffer;
    buffer.push('D'); buffer.push('E');
    char out_peek[2]; buffer.peek(out_peek);
    EXPECT_EQ(out_peek[0], 'D'); EXPECT_EQ(out_peek[1], 'E');
    std::array<char, 2> out_pop{};
    buffer.pop(out_pop);
    EXPECT_EQ(out_pop[0], 'D'); EXPECT_EQ(out_pop[1], 'E');
    EXPECT_TRUE(buffer.isEmpty());
}
TEST(RingBufferTest, FillAndOverwrite) {
    RingBuffer<int, 5> buffer;
    for (int i = 0; i < 5; ++i) buffer.push(i);
    EXPECT_TRUE(buffer.isFull());
    buffer.push(99);
    EXPECT_EQ(buffer.usedSlots(), 5u);
}
TEST(RingBufferTest, Clear) {
    RingBuffer<int, 5> buffer;
    buffer.push(1); buffer.push(2); buffer.push(3);
    buffer.clear();
    EXPECT_TRUE(buffer.isEmpty());
    buffer.push(42);
    EXPECT_EQ(buffer.pop(), 42);
}
TEST(RingBufferTest, PeekRefAndDiscard) {
    RingBuffer<int, 5> buffer;
    buffer.push(10); buffer.push(20);
    EXPECT_EQ(buffer.peekRef(), 10);
    buffer.discard(1);
    EXPECT_EQ(buffer.peekRef(), 20);
}
TEST(RingBufferTest, NextSlotAndHavePushed) {
    RingBuffer<int, 5> buffer;
    buffer.nextSlot() = 100;
    buffer.havePushed(1);
    EXPECT_EQ(buffer.pop(), 100);
}
