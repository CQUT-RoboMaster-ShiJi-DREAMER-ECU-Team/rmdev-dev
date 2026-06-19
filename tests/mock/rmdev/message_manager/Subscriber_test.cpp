#include "host_test_pch.hpp"
#include <memory>
#include "MockQueue.hpp"
#if EMDEVIF_USE_MODULES
    import rmdev.message_manager.subscriber;
#else
    #include "rmdev/message_manager/subscriber.hpp"
#endif
#if EMDEVIF_USE_MODULES
    import rmdev.message_manager.topic;
#else
    #include "rmdev/message_manager/topic.hpp"
#endif
using namespace rmdev;
using namespace emdevif;

TEST(SubscriberTest, PopRemoves) {
    Topic<MockQueue<int,5>, std::allocator> topic{"test"};
    auto sub = topic.addSubscriber();
    topic.push(false,1); topic.push(false,2);
    EXPECT_EQ(sub.storeCount(),2u);
    int v=0;
    EXPECT_EQ(static_cast<int>(sub.pop(false,v)),static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(v,1); EXPECT_EQ(sub.storeCount(),1u);
    EXPECT_EQ(static_cast<int>(sub.pop(false,v)),static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(v,2); EXPECT_EQ(sub.storeCount(),0u);
}
TEST(SubscriberTest, PeekSlot) {
    Topic<MockSlot<int>, std::allocator> topic{"test"};
    auto sub = topic.addSubscriber();
    topic.forcePush(false,42);
    int v=0;
    EXPECT_EQ(static_cast<int>(sub.peek(false,v)),static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(v,42); EXPECT_EQ(sub.storeCount(),1u);
}
TEST(SubscriberTest, Clear) {
    Topic<MockQueue<int,5>, std::allocator> topic{"test"};
    auto sub = topic.addSubscriber();
    topic.push(false,1); topic.push(false,2);
    sub.clear(); EXPECT_EQ(sub.storeCount(),0u);
}
TEST(SubscriberTest, Capacity) {
    Topic<MockQueue<int,5>, std::allocator> topic{"test"};
    auto sub = topic.addSubscriber();
    topic.push(false,1);
    EXPECT_EQ(sub.storeCount(),1u); EXPECT_EQ(sub.remainCount(),4u);
    EXPECT_EQ(sub.maxItemCount(),5u);
}
