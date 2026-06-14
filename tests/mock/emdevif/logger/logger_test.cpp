#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
extern "C" int emdevif_user_vsnprintf(char* b, size_t s, const char* f, std::va_list a) { return vsnprintf(b,s,f,a); }
extern "C" void emdevif_user_log_output(const char*, size_t) {}
#include "emdevif/logger/sync.hpp"

static int test_init_deinit() {
    auto ec = emdevif::logger::detail::logInit(emdevif_user_vsnprintf);
    if(static_cast<int>(ec)!=0) { printf("FAIL: init\n"); return 1; }
    emdevif::logger::detail::logDeInit();
    printf("PASS: logger init/deinit\n"); return 0;
}
int main() {
    int f=0; f+=test_init_deinit();
    if(f>0) { printf("\n%d FAILED\n",f); return 1; }
    printf("\nAll PASSED\n"); return 0;
}
