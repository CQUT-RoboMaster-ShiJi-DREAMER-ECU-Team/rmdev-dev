#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include "emdevif/peripheral/model/can.hpp"
#include "emdevif/peripheral/model/gpio.hpp"
#include "emdevif/peripheral/model/serial.hpp"
#include "emdevif/core/error_handler.hpp"

static int g_can=0, g_gpio=0;
static emdevif::ErrorCode m_can_tx(bool,void*,const emdevif::CanModel::DataHeader&,std::span<const uint8_t>){ ++g_can; return emdevif::ErrorCode::Success; }
static void m_gpio_w(void*,uint_fast8_t s){ g_gpio=s; }
static uint_fast8_t m_gpio_r(void*){ return (uint_fast8_t)g_gpio; }

static int test_can() {
    emdevif::CanModel::Instance can{.transmit_function=m_can_tx};
    std::vector<uint8_t> d={1,2}; emdevif::CanModel::DataHeader h{};
    auto ec=can.transmit_function(false,can.handle,h,d);
    if(static_cast<int>(ec)!=0||g_can!=1){ printf("FAIL: can\n"); return 1; }
    printf("PASS: can model\n"); return 0;
}
static int test_gpio() {
    emdevif::GpioModel::Instance g{.write_function=m_gpio_w,.read_function=m_gpio_r};
    g_gpio=0; g.write_function(g.handle,emdevif::GpioModel::Set);
    if(g_gpio!=1){ printf("FAIL: gpio write\n"); return 1; }
    if(g.read_function(g.handle)!=1){ printf("FAIL: gpio read\n"); return 1; }
    printf("PASS: gpio model\n"); return 0;
}
int main() {
    int f=0; f+=test_can(); f+=test_gpio();
    if(f){ printf("\n%d FAILED\n",f); return 1; } printf("\nAll PASSED\n"); return 0;
}
