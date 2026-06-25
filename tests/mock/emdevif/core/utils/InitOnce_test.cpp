#include "host_test_pch.hpp"
#include <string>

#if EMDEVIF_USE_MODULES
import emdevif.core.error_handler;
#else
#include "emdevif/core/error_handler.hpp"
#endif
#if EMDEVIF_USE_MODULES
import emdevif.core.utils.init_once;
#else
#include "emdevif/core/utils/init_once.hpp"
#endif

using namespace emdevif;

TEST(InitOnceTest, DefaultAndInit)
{
    InitOnce<int> io;
    EXPECT_EQ(io(), 0);
    EXPECT_EQ(static_cast<int>(io.init(42)), static_cast<int>(ErrorCode::Success));
    EXPECT_EQ(io(), 42);
    EXPECT_EQ(static_cast<int>(io.init(99)), static_cast<int>(ErrorCode::AlreadyExists));
    EXPECT_EQ(io(), 42);
}
TEST(InitOnceTest, ValueConstruct)
{
    InitOnce<int> io(42);
    EXPECT_EQ(io(), 42);
}
TEST(InitOnceTest, ImplicitConvert)
{
    InitOnce<int> io(42);
    int x = io;
    EXPECT_EQ(x, 42);
}
TEST(InitOnceTest, NonTrivialType)
{
    InitOnce<std::string> io;
    EXPECT_EQ(io(), "");
    io.init("hello");
    EXPECT_EQ(io(), "hello");
    io.init("world");
    EXPECT_EQ(io(), "hello");
}
