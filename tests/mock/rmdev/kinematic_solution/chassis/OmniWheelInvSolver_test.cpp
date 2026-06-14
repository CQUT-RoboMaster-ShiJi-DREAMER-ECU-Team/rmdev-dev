#include "host_test_pch.hpp"
#include "rmdev/kinematic_solution/chassis/OmniWheelInvSolver.hpp"
using namespace rmdev;

class OWISTest : public ::testing::Test {
protected:
    OmniWheelInvSolver<float,4> solver{0.5f};
};

TEST_F(OWISTest, Stop) {
    auto r = solver.solve({.vx=0,.vy=0,.omega=0});
    EXPECT_FLOAT_EQ(r.fr, 0); EXPECT_FLOAT_EQ(r.fl, r.fr);
}
TEST_F(OWISTest, Forward) {
    auto r = solver.solve({.vx=1,.vy=0,.omega=0});
    EXPECT_GT(r.fl,0); EXPECT_GT(r.fr,0);
    EXPECT_FLOAT_EQ(r.fl, r.fr); EXPECT_FLOAT_EQ(r.bl, r.fr);
}
TEST_F(OWISTest, Back) {
    auto r = solver.solve({.vx=-1,.vy=0,.omega=0});
    EXPECT_LT(r.fl,0); EXPECT_LT(r.fr,0);
}
TEST_F(OWISTest, Left) {
    auto r = solver.solve({.vx=0,.vy=1,.omega=0});
    EXPECT_LT(r.fl,0); EXPECT_GT(r.fr,0);
    EXPECT_FLOAT_EQ(-r.fl, r.fr); EXPECT_FLOAT_EQ(-r.br, r.fr);
}
TEST_F(OWISTest, Rotating) {
    auto r = solver.solve({.vx=0,.vy=0,.omega=1});
    EXPECT_LT(r.fl,0); EXPECT_GT(r.fr,0);
    r = solver.solve({.vx=0,.vy=0,.omega=-1});
    EXPECT_GT(r.fl,0); EXPECT_LT(r.fr,0);
}
