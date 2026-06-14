#include "host_test_pch.hpp"
#include <cstdint>
#include <vector>
#include "rmdev/debug_assistance/vofa.hpp"
using namespace rmdev::debug_assistance::vofa;

TEST(VOFATest, FrameTail) {
    auto tail = JustFloat::frame_tail;
    EXPECT_EQ(tail.size(), 4u);
    auto it = tail.begin();
    EXPECT_EQ(*it++,0x00); EXPECT_EQ(*it++,0x00);
    EXPECT_EQ(*it++,0x80); EXPECT_EQ(*it,0x7f);
}
TEST(VOFATest, AppendFrameTail) {
    std::vector<float> buf(10,0.f); buf[0]=1; buf[1]=2; buf[2]=3;
    auto r = JustFloat::appendFrameTail(std::span(buf),3);
    EXPECT_EQ(r.size(), 3*sizeof(float)+4);
    uint8_t t[]={0,0,0x80,0x7f};
    for(int i=0;i<4;++i) EXPECT_EQ(r[r.size()-4+i], t[i]);
}
TEST(VOFATest, ProcessData) {
    std::vector<float> buf(10,0.f); buf[0]=0; buf[1]=1;
    auto r = JustFloat::processData(std::span(buf),2);
    EXPECT_EQ(r.size(), 2u*sizeof(float)+4);
    uint8_t t[]={0,0,0x80,0x7f};
    for(int i=0;i<4;++i) EXPECT_EQ(r[r.size()-4+i], t[i]);
}
