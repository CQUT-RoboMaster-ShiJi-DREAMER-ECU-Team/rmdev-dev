/**
 * @file stm32_test_impl.cpp
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

#include "printf.h"

#include "emdevif_test_framework.h"

#include "emdevif/attributes_and_useful_macros.h"
#include "emdevif/fatal_handler.h"
#include "emdevif/line_separator.h"

import emdevif.errorHandler;
import emdevif.sys.thread;
import emdevif.peripheral.serial;
import emdevif.userDeclares;
import emdevif.logger;

#ifdef ENABLE_AFTER_UNIT_TEST_DEMO
import afterUnitTestDemo;
#endif

emdevif::Serial test_tx_serial{"test transmit serial"};

static char printf_buffer[512];

static constexpr auto DEFAULT_TASK_STACK_DEPTH = 1024U;
static emdevif::Thread::StaticInstance<DEFAULT_TASK_STACK_DEPTH> default_task_static_instance;
static emdevif::Thread default_task;

EMDEVIF_NO_RETURN static void osStartDefaultTask(void* arguments);

static void test_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    const auto len = ::vsnprintf(printf_buffer, std::size(printf_buffer), format, args);
    va_end(args);
    if (len < 0) {
        EMDEVIF_FATAL_HANDLER("Failed to format string in test_printf.");
    }

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
        test_printf("emdevif: Assert failed at %s:%d in function `%s\' with expression `%s\' is false. ",
                    file,
                    line,
                    func_name,
                    condition_name);
        test_printf("Message: %s" EMDEVIF_LINE_SEPARATOR, message);
    });

    default_task = emdevif::Thread::create({.name = "DefaultTask",
                                            .priority = emdevif::Thread::Priority::Max,
                                            .static_instance = &default_task_static_instance,
                                            .stack_size = DEFAULT_TASK_STACK_DEPTH},
                                           osStartDefaultTask,
                                           nullptr);
    if (default_task.getHandle() == nullptr) {
        EMDEVIF_FATAL_HANDLER("Failed to create default task.");
    }

    // 程序不应当执行到此处
    emdevif::terminate();
}

extern "C" void emdevif_testEntry(void);
#if (defined(BUILD_RMDEV) && BUILD_RMDEV)
extern "C" void rmdev_testEntry(void);
#endif

EMDEVIF_NO_RETURN static void osStartDefaultTask(void* arguments)
{
    EMDEVIF_UNUSED(arguments);

    test_printf("    emdevif test begin..." EMDEVIF_LINE_SEPARATOR);
    emdevif_test_Callbacks callbacks = {
        .printfCallback = test_printf,
        .testEntryCallback = emdevif_testEntry,
        .testFinishCallback =
            [](const emdevif_test_ErrorCode ec) {
                if (ec != EMDEVIF_TEST_ALL_PASSED) {
                    EMDEVIF_FATAL_HANDLER("Error occurred from emdevif_test_framework exit with code %d.", ec);
                }
            },
        .errorCallback = nullptr};
    emdevif_test_framework_main(EMDEVIF_LINE_SEPARATOR, &callbacks, nullptr);

#if (defined(BUILD_RMDEV) && BUILD_RMDEV)
    test_printf("    rmdev test begin..." EMDEVIF_LINE_SEPARATOR);
    callbacks.testEntryCallback = rmdev_testEntry;
    emdevif_test_framework_main(EMDEVIF_LINE_SEPARATOR, &callbacks, nullptr);
#endif

#ifdef ENABLE_AFTER_UNIT_TEST_DEMO
    demoEntry();
#endif

    if (default_task.getHandle() == nullptr) {
        EMDEVIF_FATAL_HANDLER("Default task Should not be null here!");
    }

    // 程序不应当执行到此处
    emdevif::terminate();
}
