#include "host_test_pch.hpp"

#ifdef EMDEVIF_USE_MODULES
import emdevif.timeline;
#else
#include "emdevif/timeline.hpp"
#endif

using namespace emdevif;

TEST(TimelineTest, GetMicrosecondsReturnsIncrementingValue)
{
    auto t1 = Timeline::getMicroseconds();
    auto t2 = Timeline::getMicroseconds();
    EXPECT_GT(t2, t1);
}

TEST(TimelineTest, UpdateStoresCurrentTime)
{
    Timeline tl;
    tl.update();
    auto stored = tl();
    EXPECT_GT(stored, 0);
}

TEST(TimelineTest, DurationMeasuresInterval)
{
    Duration<float> dur;
    dur.update();

    auto before = Timeline::getMicroseconds();
    auto elapsed = dur.getMicroDuration();
    auto after = Timeline::getMicroseconds();

    EXPECT_GT(elapsed, 0);
    EXPECT_LE(elapsed, after - before + 1);
}

TEST(TimelineTest, PauseDelayMsCompletes)
{
    Timeline::pauseDelayMs(1);
    SUCCEED();
}
