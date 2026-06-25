#include "host_test_pch.hpp"

#include "emdevif/peripheral/peripheral_handle_map.hpp"

using namespace emdevif;

TEST(PeripheralHandleMapTest, FindHandleReturnsValidPointer)
{
    auto* gpio = PeripheralHandleMap::findHandle("test_gpio");
    EXPECT_NE(gpio, nullptr);

    auto* serial = PeripheralHandleMap::findHandle("test_serial");
    EXPECT_NE(serial, nullptr);
}

TEST(PeripheralHandleMapTest, FindHandleReturnsNullptrForUnknownName)
{
    auto* result = PeripheralHandleMap::findHandle("nonexistent");
    EXPECT_EQ(result, nullptr);
}
