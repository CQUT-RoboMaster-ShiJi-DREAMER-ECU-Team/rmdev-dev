#include "host_test_pch.hpp"
#include <cmath>
#include "rmdev/control_algorithm/pid.hpp"
using namespace rmdev;

TEST(PIDTest, ProportionalOnly) {
    Pid<float> pid({.kp=2.f,.ki=0.f,.kd=0.f}, 100.f, 100.f);
    EXPECT_FLOAT_EQ(pid.calc(10.f,0.f), 20.f);
}
TEST(PIDTest, IntegralAccumulation) {
    Pid<float> pid({.kp=0.f,.ki=.5f,.kd=0.f}, 100.f, 100.f);
    pid.calc(10.f,0.f); pid.calc(10.f,0.f);
    EXPECT_FLOAT_EQ(pid.calc(10.f,0.f), 15.f);
}
TEST(PIDTest, IntegralLimit) {
    Pid<float> pid({.kp=0.f,.ki=10.f,.kd=0.f}, 100.f, 5.f);
    for(int i=0;i<10;++i) pid.calc(10.f,0.f);
    EXPECT_LE(std::abs(pid.getOutput()), 5.1f);
}
TEST(PIDTest, DerivativeTerm) {
    Pid<float> pid({.kp=0.f,.ki=0.f,.kd=1.f}, 100.f, 100.f);
    pid.calc(0.f,0.f);
    EXPECT_NEAR(pid.calc(0.f,10.f), -10.f, .01f);
}
TEST(PIDTest, Deadband) {
    Pid<float> pid({.kp=1.f,.ki=0.f,.kd=0.f}, 100.f, 100.f, PID_NONE, .5f);
    EXPECT_FLOAT_EQ(pid.calc(10.f,9.7f), 0.f);
}
TEST(PIDTest, OutputLimit) {
    Pid<float> pid({.kp=100.f,.ki=0.f,.kd=0.f}, 10.f, 100.f);
    EXPECT_LE(std::abs(pid.calc(100.f,0.f)), 10.01f);
}
TEST(PIDTest, DerivOnMeas) {
    auto imp = static_cast<PidImprove>(PID_DERIVATIVE_ON_MEASUREMENT);
    Pid<float> pid({.kp=0.f,.ki=0.f,.kd=1.f}, 100.f, 100.f, imp);
    pid.calc(0.f,0.f);
    EXPECT_NEAR(pid.calc(0.f,10.f), -10.f, .01f);
}
TEST(PIDTest, ParamChange) {
    Pid<float> pid({.kp=1.f,.ki=0.f,.kd=0.f}, 100.f, 100.f);
    pid.setKp(2.f); EXPECT_FLOAT_EQ(pid.getKp(), 2.f);
    pid.setKi(.5f); EXPECT_FLOAT_EQ(pid.getKi(), .5f);
    pid.setKd(.1f); EXPECT_FLOAT_EQ(pid.getKd(), .1f);
    EXPECT_GT(std::abs(pid.calc(10.f,0.f)), 0.f);
}
TEST(PIDTest, OperatorCall) {
    Pid<float> pid({.kp=1.f,.ki=0.f,.kd=0.f}, 100.f, 100.f);
    EXPECT_FLOAT_EQ(pid(10.f,5.f), 5.f);
    EXPECT_FLOAT_EQ(pid(), pid.getOutput());
}
