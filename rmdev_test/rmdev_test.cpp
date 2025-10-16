/**
 * @file rmdev_test.cpp
 * @author DuYicheng
 * @date 2025-07-30
 * @brief 测试函数的实现
 */

#include "rmdev_test.h"

#include <cstdarg>
#include <cstring>
#include <cstdint>

#include <iterator>
#include <limits>
#include <string_view>

#include "printf.h"
#include "main.h"

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

extern "C" void rmdev_testEntry(void);  // NOLINT(*-redundant-void-arg)

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

extern "C" EMDEVIF_NO_RETURN void testEntry(void)
{
    emdevif::registerTerminateFunction([]() noexcept {
        __disable_irq();
        while (true) {
        }
    });

    emdevif::registerFatalHandler([](const char* file, const int line, const char* message) noexcept {
        test_printf("emdevif: Fatal touched at %s:%d" EMDEVIF_LINE_SEPARATOR EMDEVIF_LINE_SEPARATOR, file, line);
        test_printf("Message: %s" EMDEVIF_LINE_SEPARATOR, message);
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

    emdevif::Logger::init();
    emdevif::Logger::registerVSPrintfFunction([](char* dst, const char* format, std::va_list args) {
        return ::vsnprintf(dst, std::numeric_limits<std::size_t>::max(), format, args);
    });

    default_task = emdevif::Thread::create({.name = "DefaultTask",
                                            .priority = emdevif::Thread::Priority::Max,
                                            .static_instance = &default_task_static_instance,
                                            .stack_size = DEFAULT_TASK_STACK_DEPTH},
                                           osStartDefaultTask,
                                           nullptr);
    if (!default_task.getHandle().has_value()) {
        EMDEVIF_FATAL_HANDLER("Failed to create default task.");
    }

    emdevif::Thread::startScheduler();

    // 程序不应当执行到此处
    while (true) {
    }
}

EMDEVIF_NO_RETURN static void osStartDefaultTask(void* arguments)
{
    EMDEVIF_UNUSED(arguments);

    constexpr emdevif_test_Callbacks callbacks = {
        .printfCallback = test_printf,
        .testEntryCallback = rmdev_testEntry,
        .testFinishCallback =
            [](const emdevif_test_ErrorCode ec) {
                if (ec != EMDEVIF_TEST_ALL_PASSED) {
                    using namespace std::literals;

                    emdevif::err_msg.clear();

                    char ec_buffer[8];
                    ::snprintf(ec_buffer, std::size(ec_buffer), "%d", ec);
                    emdevif::err_msg << "Error occured from emdevif_test_framework with code "sv << ec_buffer << "."sv;
                    EMDEVIF_FATAL_HANDLER(emdevif::err_msg.c_str());
                }
            },
        .errorCallback = nullptr};
    emdevif_test_framework_main(EMDEVIF_LINE_SEPARATOR, &callbacks, nullptr);

#ifdef ENABLE_AFTER_UNIT_TEST_DEMO
    demoEntry();
#endif

    if (!default_task.getHandle().has_value()) {
        EMDEVIF_FATAL_HANDLER("Default task Should not be null here!");
    }
    emdevif::Thread::suspend(*default_task.getHandle());
    while (true) {
        emdevif::Thread::delay(emdevif::Thread::maxDelay());
    }
}
