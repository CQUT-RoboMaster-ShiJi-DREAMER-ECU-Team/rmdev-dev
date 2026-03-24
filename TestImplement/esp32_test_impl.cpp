/**
 * @file esp32_test_impl.cpp
 * @author DuYicheng
 * @date 2025-11-26
 * @brief ESP32 测试函数的实现
 */

#include "rmdev_test.h"

#include <cstdarg>
#include <cstring>
#include <cstdint>

#include <iterator>
#include <limits>
#include <string_view>

#include "esp_log.h"
#include "esp_task_wdt.h"

#include "printf.h"

#include "emdevif_test_framework.h"

#include "emdevif/attributes_and_useful_macros.h"
#include "emdevif/fatal_handler.h"
#include "emdevif/line_separator.h"

import emdevif.errorHandler;
import emdevif.sys.thread;
import emdevif.peripheral.serial;
import emdevif.user_declares;
import emdevif.logger;

#ifdef ENABLE_AFTER_UNIT_TEST_DEMO
import afterUnitTestDemo;
#endif

emdevif::Serial test_tx_serial{"test transmit serial"};

static char printf_buffer[512];

EMDEVIF_NO_RETURN static void testImpl(void);

static void test_printf(const char* format, ...)
{
    constexpr auto log_tag = "esp32_test_impl.cpp test_printf";

    va_list args;
    va_start(args, format);

    const auto len = ::vsnprintf(printf_buffer, std::size(printf_buffer), format, args);
    va_end(args);
    if (len < 0) {
        EMDEVIF_FATAL_HANDLER("Failed to format string in test_printf(buffer len: %d).", len);
    }
    printf_buffer[len] = '\0';

    ESP_LOGV(log_tag, "buffer: \"%s\"" EMDEVIF_LINE_SEPARATOR "len=%d", printf_buffer, len);
    const auto tx_ptr = reinterpret_cast<const uint8_t*>(printf_buffer);
    test_tx_serial.transmit(false, {tx_ptr, static_cast<std::size_t>(len)}, emdevif::Serial::max_delay);
}

// 无需初始化，该函数可调用也可以不调用
extern "C" void testInit(void* argument, ...) {}

extern "C" EMDEVIF_NO_RETURN void testEntry(void)
{
    emdevif::registerTerminateFunction([]() noexcept {
        test::terminateImpl();
        while (true) {
        }
    });

    emdevif::registerFatalHandler([](const char* file, const int line, const char* format, std::va_list args) noexcept {
        ESP_LOGW("emdevif fatalHandler", "Called EMDEVIF_FATAL_HANDLER in %s:%d", file, line);

        test_printf("emdevif: Fatal touched at %s:%d" EMDEVIF_LINE_SEPARATOR EMDEVIF_LINE_SEPARATOR, file, line);

        static char buffer[256];
        ::snprintf(buffer, std::size(buffer), format, args);

        test_printf("Message: %s" EMDEVIF_LINE_SEPARATOR, buffer);
    });

    emdevif::registerAssertFailedHandler([](const char* file,
                                            const int line,
                                            const char* func_name,
                                            const char* condition_name,
                                            const char* message) noexcept {
        ESP_LOGW("emdevif assertFailedHandler", "assert failed at %s:%d", file, line);

        test_printf("emdevif: Assert failed at %s:%d in function `%s\' with expression `%s\' is false. ",
                    file,
                    line,
                    func_name,
                    condition_name);
        test_printf("Message: %s" EMDEVIF_LINE_SEPARATOR, message);
    });

    ESP_ERROR_CHECK(esp_task_wdt_deinit());

    testImpl();

    // 程序不应当执行到此处
    emdevif::terminate();
}

extern "C" void emdevif_testEntry(void);
#if (defined(BUILD_RMDEV) && BUILD_RMDEV)
extern "C" void rmdev_testEntry(void);
#endif

EMDEVIF_NO_RETURN static void testImpl(void)
{
    ESP_LOGI(__func__, "emdevif test begin.");

    test_printf("    emdevif test begin..." EMDEVIF_LINE_SEPARATOR);
    emdevif_test_Callbacks callbacks = {.printfCallback = test_printf,
                                        .testEntryCallback = emdevif_testEntry,
                                        .testFinishCallback =
                                            [](const emdevif_test_ErrorCode ec) {
                                                if (ec != EMDEVIF_TEST_ALL_PASSED) {
                                                    EMDEVIF_FATAL_HANDLER(
                                                        "Error occurred from emdevif_test_framework exit with code %d.",
                                                        static_cast<int>(ec));
                                                }
                                            },
                                        .errorCallback = nullptr};
    emdevif_test_framework_main(EMDEVIF_LINE_SEPARATOR, &callbacks, nullptr);

    ESP_LOGI(__func__, "emdevif test end.");

#if (defined(BUILD_RMDEV) && BUILD_RMDEV)
    ESP_LOGI(__func__, "rmdev test begin.");

    test_printf("    rmdev test begin..." EMDEVIF_LINE_SEPARATOR);
    callbacks.testEntryCallback = rmdev_testEntry;
    emdevif_test_framework_main(EMDEVIF_LINE_SEPARATOR, &callbacks, nullptr);

    ESP_LOGI(__func__, "rmdev test end.");
#endif

#ifdef ENABLE_AFTER_UNIT_TEST_DEMO
    ESP_LOGI(__func__, "Test finished, entering into demo...");
    demoEntry();
    ESP_LOGI(__func__, "Demo finished.");
#endif

    ESP_LOGI(__func__, "Test finished, terminating...");
    emdevif::terminate();
}
