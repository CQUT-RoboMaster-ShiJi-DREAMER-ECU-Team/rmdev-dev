#include "host_test_pch.hpp"
#include "rmdev/kinematic_solution/chassis/FourWheelChassisSolver.hpp"
using namespace rmdev;

TEST(ChassisTest, TargetSpeedIndex) {
    ChassisTargetSpeed<float> s{.vx=1,.vy=2,.omega=3};
    EXPECT_FLOAT_EQ(s[WHEEL_VX], 1);
    EXPECT_FLOAT_EQ(s[WHEEL_VY], 2);
    EXPECT_FLOAT_EQ(s[WHEEL_OMEGA], 3);
}
TEST(ChassisTest, TargetSpeedMut) {
    ChassisTargetSpeed<float> s{};
    s[WHEEL_VX]=10; s[WHEEL_VY]=20; s[WHEEL_OMEGA]=30;
    EXPECT_FLOAT_EQ(s.vx,10); EXPECT_FLOAT_EQ(s.vy,20); EXPECT_FLOAT_EQ(s.omega,30);
}
TEST(ChassisTest, WheelsSpeedIndex) {
    ChassisWheelsSpeed<float> w{.fl=1,.bl=2,.br=3,.fr=4};
    EXPECT_FLOAT_EQ(w[WHEEL_FL],1); EXPECT_FLOAT_EQ(w[WHEEL_BL],2);
    EXPECT_FLOAT_EQ(w[WHEEL_BR],3); EXPECT_FLOAT_EQ(w[WHEEL_FR],4);
}
