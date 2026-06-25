#include <csetjmp>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "emdevif/core/fatal_handler.h"

#if EMDEVIF_USE_MODULES
import emdevif.core.error_handler;
#else
#include "emdevif/core/error_handler.hpp"
#endif

using namespace emdevif;

static jmp_buf g_jb;
static bool g_term = false, g_fatal = false, g_assert = false;
static char g_ff[256] = {};
static int g_fl = 0;
static char g_af[256] = {}, g_as[256] = {};

static void mock_term()
{
    g_term = true;
    longjmp(g_jb, 1);
}
static void mock_fatal(const char* f, int l, const char* fmt, std::va_list a)
{
    g_fatal = true;
    if (f) {
        std::strncpy(g_ff, f, 255);
    }
    g_fl = l;
}
static void mock_assert(const char* f, int l, const char*, const char* c, const char*)
{
    g_assert = true;
    if (f) {
        std::strncpy(g_af, f, 255);
    }
    if (c) {
        std::strncpy(g_as, c, 255);
    }
    longjmp(g_jb, 1);
}

static int test_term()
{
    registerTerminateFunction(mock_term);
    if (setjmp(g_jb) == 0) {
        terminate();
        return 1;
    }
    if (!g_term) {
        printf("FAIL: terminate\n");
        return 1;
    }
    printf("PASS: terminate\n");
    return 0;
}
static int test_fatal()
{
    registerFatalHandler(mock_fatal);
    registerTerminateFunction(mock_term);
    if (setjmp(g_jb) == 0) {
        fatalHandler("test.cpp", 42, "error %d", 123);
        return 1;
    }
    if (!g_fatal || std::strcmp(g_ff, "test.cpp") != 0 || g_fl != 42) {
        printf("FAIL: fatal\n");
        return 1;
    }
    printf("PASS: fatal\n");
    return 0;
}
static int test_assert()
{
    registerAssertFailedHandler(mock_assert);
    registerTerminateFunction(mock_term);
    if (setjmp(g_jb) == 0) {
        EMDEVIF_ASSERT(false);
        printf("FAIL: assert\n");
        return 1;
    }
    if (!g_assert) {
        printf("FAIL: assert not fired\n");
        return 1;
    }
    g_assert = false;
    EMDEVIF_ASSERT(true);
    if (g_assert) {
        printf("FAIL: assert(true) fired\n");
        return 1;
    }
    printf("PASS: assert\n");
    return 0;
}
static int test_ec()
{
    if (ErrorCode::Success != ErrorCode::Success) {
        printf("FAIL: ec\n");
        return 1;
    }
    if (ErrorCode::Timeout == ErrorCode::Success) {
        printf("FAIL: ec cmp\n");
        return 1;
    }
    printf("PASS: error_code\n");
    return 0;
}
int main()
{
    int f = 0;
    f += test_term();
    f += test_fatal();
    f += test_assert();
    f += test_ec();
    if (f > 0) {
        printf("\n%d FAILED\n", f);
        return 1;
    }
    printf("\nAll PASSED\n");
    return 0;
}
