#include "host_test_pch.hpp"
#include <cstdint>

#ifdef __cpp_exceptions
#include <stdexcept>
#endif

#include <utility>

#if EMDEVIF_USE_MODULES
import emdevif.core.error_handler;
#else
#include "emdevif/core/error_handler.hpp"
#endif
#if EMDEVIF_USE_MODULES
import emdevif.core.resource_guard;
#else
#include "emdevif/core/resource_guard.hpp"
#endif

using namespace emdevif;

static int g_cnt = 0;
void inc() noexcept
{
    ++g_cnt;
}
void reset() noexcept
{
    g_cnt = 0;
}

TEST(DeferTest, BasicExecution)
{
    reset();
    {
        Defer d(&inc);
        EXPECT_EQ(g_cnt, 0);
    }
    EXPECT_EQ(g_cnt, 1);
}
TEST(DeferTest, LambdaCapture)
{
    int v = 0;
    {
        Defer d([&v]() noexcept { v = 42; });
    }
    EXPECT_EQ(v, 42);
}
TEST(DeferTest, ReverseOrder)
{
    int ord[3] = {0, 0, 0}, step = 0;
    {
        Defer d1([&]() noexcept { ord[step++] = 1; });
        Defer d2([&]() noexcept { ord[step++] = 2; });
        Defer d3([&]() noexcept { ord[step++] = 3; });
    }
    EXPECT_EQ(ord[0], 3);
    EXPECT_EQ(ord[1], 2);
    EXPECT_EQ(ord[2], 1);
}
TEST(DeferTest, NestedDefer)
{
    int v = 0;
    {
        Defer d1([&v]() noexcept { v += 10; });
        {
            Defer d2([&v]() noexcept { v += 1; });
            EXPECT_EQ(v, 0);
        }
    }
    EXPECT_EQ(v, 11);
}
TEST(DeferTest, ConstexprDefer)
{
    constexpr auto f = []() constexpr -> bool {
        int flag = 0;
        {
            Defer d([&flag]() noexcept { flag = 1; });
            if (flag != 0) {
                return false;
            }
        }
        return flag == 1;
    };
    EXPECT_TRUE(f());
    static_assert(f());
}

class MockLock
{
public:
    int lk = 0, tlk = 0, ulk = 0;
    ErrorCode lret = ErrorCode::Success, tlret = ErrorCode::Success;
    ErrorCode lock() noexcept
    {
        ++lk;
        return lret;
    }
    ErrorCode lock(int) noexcept
    {
        ++lk;
        return lret;
    }
    ErrorCode try_lock() noexcept
    {
        ++tlk;
        return tlret;
    }
    void unlock() noexcept
    {
        ++ulk;
    }
};

#ifdef __cpp_exceptions
TEST(LockGuardTest, AutoLock)
{
    MockLock lock;
    {
        LockGuard guard(lock);
        EXPECT_EQ(lock.lk, 1);
    }
    EXPECT_EQ(lock.ulk, 1);
}
#endif
TEST(LockGuardTest, DoNotLock)
{
    MockLock lock;
    {
        LockGuard guard(lock_guard_do_not_lock_when_init, lock);
        EXPECT_EQ(lock.lk, 0);
    }
    EXPECT_EQ(lock.ulk, 1);
}
TEST(LockGuardTest, ManualLock)
{
    MockLock lock;
    {
        LockGuard guard(lock_guard_do_not_lock_when_init, lock);
        EXPECT_EQ(static_cast<int>(guard.lock()), static_cast<int>(ErrorCode::Success));
        EXPECT_EQ(lock.lk, 1);
    }
    EXPECT_EQ(lock.ulk, 1);
}

class MockDevice
{
public:
    bool initd = false, deinitd = false;
    ErrorCode ire = ErrorCode::Success;
    int ia = 0;
    ErrorCode init() noexcept
    {
        initd = true;
        return ire;
    }
    ErrorCode init(int a) noexcept
    {
        initd = true;
        ia = a;
        return ire;
    }
    void deInit() noexcept
    {
        deinitd = true;
    }
};

#ifdef __cpp_exceptions
TEST(InitGuardTest, AutoInit)
{
    MockDevice dev;
    {
        InitGuard guard(dev);
        EXPECT_TRUE(dev.initd);
    }
    EXPECT_TRUE(dev.deinitd);
}
#endif
TEST(InitGuardTest, DoNotInit)
{
    MockDevice dev;
    {
        InitGuard guard(init_guard_do_not_init_object_tag, dev);
        EXPECT_FALSE(dev.initd);
    }
    EXPECT_TRUE(dev.deinitd);
}
TEST(InitGuardTest, ManualInit)
{
    MockDevice dev;
    {
        InitGuard guard(init_guard_do_not_init_object_tag, dev);
        EXPECT_EQ(static_cast<int>(guard.init()), static_cast<int>(ErrorCode::Success));
        EXPECT_TRUE(dev.initd);
    }
    EXPECT_TRUE(dev.deinitd);
}

TEST(TryFinallyTest, Basic)
{
    int fr = 0;
    int r = tryFinally([&] { return 42; }, [&]() noexcept { fr = 1; });
    EXPECT_EQ(r, 42);
    EXPECT_EQ(fr, 1);
}
TEST(TryFinallyTest, VoidReturn)
{
    int fr = 0, se = 0;
    tryFinally([&] { se = 77; }, [&]() noexcept { fr = 1; });
    EXPECT_EQ(se, 77);
    EXPECT_EQ(fr, 1);
}
