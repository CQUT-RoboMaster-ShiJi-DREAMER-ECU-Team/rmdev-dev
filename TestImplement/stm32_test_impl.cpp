/**
 * @file stm32_test_impl.cpp
 * @author DuYicheng
 * @date 2025-07-30
 * @brief STM32 测试函数的实现
 */

#include "rmdev_test.h"

#include <cstdarg>
#include <cstring>
#include <cstdint>

#include <iterator>
#include <limits>
#include <string_view>

#include "emdevif_test_framework.h"

#include "emdevif/core/attributes_and_useful_macros.h"
#include "emdevif/core/fatal_handler.h"
#include "emdevif/core/line_separator.h"

static void test_printf(const char* format, ...) EMDEVIF_FORMAT_CHECK(printf, 1, 2);

#if (defined(EMDEVIF_USE_MODULES) && EMDEVIF_USE_MODULES)
import emdevif.core.error_handler;
import emdevif.system.thread;
import emdevif.peripheral.serial;
import emdevif.user_declares;
import emdevif.logger;
#else
    #include "emdevif/core/error_handler.hpp"
    #include "emdevif/system/thread.hpp"
    #include "emdevif/peripheral/serial.hpp"
    #include "emdevif/user_declares.hpp"
    #include "emdevif/logger.hpp"
#endif

#include "printf.h"

#ifdef ENABLE_AFTER_UNIT_TEST_DEMO
    #if (defined(EMDEVIF_USE_MODULES) && EMDEVIF_USE_MODULES)
import afterUnitTestDemo;
    #else
#include "demo_entry.hpp"
    #endif
#endif

static emdevif::Serial test_tx_serial{"test transmit serial"};

static char printf_buffer[512];

static constexpr auto DEFAULT_TASK_STACK_DEPTH = 1024U;
static emdevif::ThreadStaticInstance<DEFAULT_TASK_STACK_DEPTH> default_task_static_instance;
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

// ReSharper disable once CppParameterMayBeConstPtrOrRef

// 需要在 HAL_Init 之前调用
// 无需传递任何参数（第一个参数可以传空指针）
extern "C" void testInit(void* argument, ...)
{
    EMDEVIF_UNUSED(argument);

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
}

extern "C" EMDEVIF_NO_RETURN void testEntry(void)
{
    emdevif::logger::init(::vsnprintf).terminateIfNotSucceed();

    default_task = emdevif::Thread::create({.name = "DefaultTask",
                                            .priority = emdevif::ThreadPriority::Max,
                                            .static_instance = &default_task_static_instance,
                                            .stack_size = DEFAULT_TASK_STACK_DEPTH},
                                           osStartDefaultTask,
                                           nullptr);
    emdevif::terminateIfNullptr(default_task.getHandle());

    emdevif::Thread::startScheduler();

    // 程序不应当执行到此处
    while (true) {
    }
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
    emdevif::Thread::suspend(default_task.getHandle());
    while (true) {
        emdevif::Thread::delay(emdevif::Thread::maxDelay());
    }
}
