#include <cstdio>
#include <cstdlib>
#include <string_view>
static int g_u1=1, g_s2=2;
constexpr auto peripheral_handle_map = emdevif::makeStaticMap<std::string_view, void*>({{"uart1",(void*)&g_u1},{"spi2",(void*)&g_s2}});
#include "emdevif/peripheral/peripheral_handle_map.hpp"

static int test_find() {
    void* h=emdevif::PeripheralHandleMap::findHandle("uart1");
    if(h!=&g_u1){ printf("FAIL: find uart1\n"); return 1; }
    h=emdevif::PeripheralHandleMap::findHandle("nonexist");
    if(h!=nullptr){ printf("FAIL: nonexist should be null\n"); return 1; }
    printf("PASS: peripheral handle map\n"); return 0;
}
int main() { int f=0; f+=test_find(); if(f){ printf("\n%d FAILED\n",f); return 1; } printf("\nAll PASSED\n"); return 0; }
