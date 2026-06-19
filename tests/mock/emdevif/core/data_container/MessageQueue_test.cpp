#include "host_test_pch.hpp"
#include "../../../rmdev/message_manager/MockQueue.hpp"
#if EMDEVIF_USE_MODULES
    import emdevif.core.data_container.message_queue;
#else
    #include "emdevif/core/data_container/message_queue.hpp"
#endif

using namespace emdevif;

TEST(MessageQueueTest, MockQueueSatisfiesConcept) {
    static_assert(MessageQueue<MockQueue<int, 5>>);
    static_assert(MessageSlot<MockSlot<int>>);
}

TEST(MessageQueueTest, PushPop) {
    MockQueue<int, 5> q;
    EXPECT_EQ(q.storeCount(), 0u);
    EXPECT_EQ(q.maxItemCount(), 5u);
    EXPECT_EQ(static_cast<int>(q.push(false, 42)), static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(q.storeCount(), 1u);
    int val = 0;
    EXPECT_EQ(static_cast<int>(q.pop(false, val)), static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(val, 42);
    EXPECT_EQ(q.storeCount(), 0u);
}

TEST(MessageQueueTest, FullAndEmpty) {
    MockQueue<int, 3> q;
    EXPECT_EQ(q.remainCount(), 3u);
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(static_cast<int>(q.push(false, i)), static_cast<int>(ErrorCode::Success));
    }
    EXPECT_EQ(q.remainCount(), 0u);
    EXPECT_EQ(static_cast<int>(q.push(false, 99)), static_cast<int>(ErrorCode::Full));
    q.clear();
    EXPECT_EQ(q.storeCount(), 0u);
    int val = 0;
    EXPECT_EQ(static_cast<int>(q.pop(false, val)), static_cast<int>(ErrorCode::Empty));
}

TEST(MessageQueueTest, ForcePush) {
    MockQueue<int, 3> q;
    for (int i = 0; i < 3; ++i) {
        q.push(false, i);
    }
    q.forcePush(false, 99);
    EXPECT_EQ(q.storeCount(), 4u);
    EXPECT_EQ(q.remainCount(), 0u);
}

TEST(MessageQueueTest, Peek) {
    MockSlot<int> s;
    s.forcePush(false, 42);
    int val = 0;
    EXPECT_EQ(static_cast<int>(s.peek(false, val)), static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(val, 42);
    EXPECT_EQ(s.storeCount(), 1u);
    s.clear();
    EXPECT_EQ(s.storeCount(), 0u);
}
