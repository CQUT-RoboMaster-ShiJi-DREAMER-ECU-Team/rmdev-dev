#include "host_test_pch.hpp"
#include <memory>
#include "MockQueue.hpp"
#if EMDEVIF_USE_MODULES
    import rmdev.message_manager.topic;
#else
    #include "rmdev/message_manager/topic.hpp"
#endif
#if EMDEVIF_USE_MODULES
    import rmdev.message_manager.subscriber;
#else
    #include "rmdev/message_manager/subscriber.hpp"
#endif
using namespace rmdev;
using namespace emdevif;

TEST(TopicTest, Construction) {
    Topic<MockQueue<int,5>, std::allocator> topic{"test_topic"};
    EXPECT_STREQ(topic.getName().data(), "test_topic");
    EXPECT_EQ(topic.getQueueListInstance().size(), 0u);
}
TEST(TopicTest, AddSubscriber) {
    Topic<MockQueue<int,5>, std::allocator> topic{"test"};
    [[maybe_unused]] auto sub = topic.addSubscriber();
    EXPECT_EQ(topic.getQueueListInstance().size(), 1u);
}
TEST(TopicTest, PushAndPop) {
    Topic<MockQueue<int,5>, std::allocator> topic{"test"};
    auto sub = topic.addSubscriber();
    auto [ec, it] = topic.push(false, 42);
    EXPECT_EQ(static_cast<int>(ec), static_cast<int>(ErrorCode::Success));
    int val=0;
    EXPECT_EQ(static_cast<int>(sub.pop(false,val)), static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(val, 42);
}
TEST(TopicTest, PushMultiple) {
    Topic<MockQueue<int,5>, std::allocator> topic{"test"};
    auto sub = topic.addSubscriber();
    for(int i:{1,2,3,4,5}) { auto [ec,it]=topic.push(false,i); EXPECT_EQ(static_cast<int>(ec),static_cast<int>(ErrorCode::Success)); }
    for(int i=1; i<=5; ++i) { int v=0; EXPECT_EQ(static_cast<int>(sub.pop(false,v)),static_cast<int>(ErrorCode::Success)); EXPECT_EQ(v,i); }
}
TEST(TopicTest, MultiSubscribers) {
    Topic<MockQueue<int,5>, std::allocator> topic{"test"};
    auto s1=topic.addSubscriber(); auto s2=topic.addSubscriber();
    topic.push(false, 10);
    int v1=0,v2=0;
    EXPECT_EQ(static_cast<int>(s1.pop(false,v1)),static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(static_cast<int>(s2.pop(false,v2)),static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(v1,10); EXPECT_EQ(v2,10);
}
TEST(TopicTest, ForcePushSlot) {
    Topic<MockSlot<int>, std::allocator> topic{"test"};
    auto sub = topic.addSubscriber();
    auto [ec,it] = topic.forcePush(false, 99);
    EXPECT_EQ(static_cast<int>(ec),static_cast<int>(ErrorCode::Success));
    int v=0;
    EXPECT_EQ(static_cast<int>(sub.peek(false,v)),static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(v,99);
}
