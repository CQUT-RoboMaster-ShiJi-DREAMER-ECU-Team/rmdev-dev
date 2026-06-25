#include <cstdarg>
#include <cstdio>
#include <csetjmp>

#include <type_traits>
#include <string_view>
#include <utility>

#ifdef __cpp_exceptions
#include <exception>
#include <stdexcept>
#endif

#undef NDEBUG  // To test assert in release mode
#include "emdevif/core/fatal_handler.h"

#if EMDEVIF_USE_MODULES
import emdevif.core.error_handler;
#else
#include "emdevif/core/error_handler.hpp"
#endif

namespace {

template<typename T>
constexpr decltype(auto) to_underlying(T value) noexcept
    requires std::is_enum_v<T>
{
    return static_cast<std::underlying_type_t<T>>(value);
}

class TestOut
{
public:
    template<typename Func>
    void operator|(const Func& func)
    {
        static_assert(std::is_trivially_destructible_v<Func>);

        for (int i = 0; i < indent_count; ++i) {
            std::printf(" >> ");
        }
        func();
        std::putchar('\n');
    }

    void increaceIndent() noexcept
    {
        ++indent_count;
    }
    void decreaceIndent() noexcept
    {
        if (indent_count >= 1) {
            --indent_count;
        }
    }

private:
    int indent_count = 0;
} testOut;

static_assert(std::is_trivially_destructible_v<TestOut>);

enum class MockErrorType : int {
    None = 0,
    Terminate,
    Fatal,
    Assert,
};

MockErrorType last_mock_error_type = MockErrorType::None;
std::jmp_buf test_jump_buffer;

void mockTerminate() noexcept
{
    testOut | [] { std::printf("Terminate called"); };

    last_mock_error_type = MockErrorType::Terminate;
    std::longjmp(test_jump_buffer, 1);
}

void mockFatalHandler(const char* file, int line, const char* format, std::va_list args) noexcept
{
    testOut | [&] {
        std::printf("Fatal error called in file: %s, line: %d, message: ", file, line);
        std::vprintf(format, args);
    };

    last_mock_error_type = MockErrorType::Fatal;
    std::longjmp(test_jump_buffer, 1);
}

void mockAssertFailedHandler(const char* file,
                             int line,
                             const char* func_name,
                             const char* condition_name,
                             const char* message) noexcept
{
    testOut | [&] {
        std::printf("Assert failed called in file: %s, line: %d, function: %s, condition: %s, message: %s",
                    file,
                    line,
                    func_name,
                    condition_name,
                    message);
    };

    last_mock_error_type = MockErrorType::Assert;
    std::longjmp(test_jump_buffer, 1);
}

}  // namespace

// Tests
namespace {

int testTerminate() noexcept
{
    std::printf("Testing terminate...\n");
    testOut.increaceIndent();

    emdevif::registerTerminateFunction(mockTerminate);

    bool test_passed = false;
    if (setjmp(test_jump_buffer) == 0) {
        emdevif::terminate();
    }
    else {
        testOut.increaceIndent();
        if (last_mock_error_type == MockErrorType::Terminate) {
            testOut | [] { std::printf("PASS"); };
            test_passed = true;
        }
        else {
            testOut | [] { std::printf("FAILED, last_mock_error_type=%d", ::to_underlying(last_mock_error_type)); };
        }
        testOut.decreaceIndent();
    }

    testOut.decreaceIndent();
    last_mock_error_type = MockErrorType::None;

    return test_passed ? 0 : 1;
}

int testFatalHandler() noexcept
{
    std::printf("Testing fatal handler...\n");
    testOut.increaceIndent();

    emdevif::registerFatalHandler(mockFatalHandler);

    bool test_passed = false;
    if (setjmp(test_jump_buffer) == 0) {
        EMDEVIF_FATAL_HANDLER("Test fatal error message: %d", 42);
    }
    else {
        testOut.increaceIndent();
        if (last_mock_error_type == MockErrorType::Fatal) {
            testOut | [] { std::printf("PASS"); };
            test_passed = true;
        }
        else {
            testOut | [] { std::printf("FAILED, last_mock_error_type=%d", ::to_underlying(last_mock_error_type)); };
        }
        testOut.decreaceIndent();
    }

    testOut.decreaceIndent();
    last_mock_error_type = MockErrorType::None;

    return test_passed ? 0 : 1;
}

int testAssertFailedHandler() noexcept
{
    std::printf("Testing assert failed handler...\n");
    testOut.increaceIndent();

    emdevif::registerAssertFailedHandler(mockAssertFailedHandler);

    bool test_passed = false;
    if (setjmp(test_jump_buffer) == 0) {
        EMDEVIF_ASSERT(false, "Test assert failed message");
    }
    else {
        testOut.increaceIndent();
        if (last_mock_error_type == MockErrorType::Assert) {
            testOut | [] { std::printf("PASS"); };
            test_passed = true;
        }
        else {
            testOut | [] { std::printf("FAILED, last_mock_error_type=%d", ::to_underlying(last_mock_error_type)); };
        }
        testOut.decreaceIndent();
    }

    testOut.decreaceIndent();
    last_mock_error_type = MockErrorType::None;

    return test_passed ? 0 : 1;
}

#ifdef __cpp_exceptions

int testErrorWithCodeException() noexcept
{
    std::printf("Testing error with code exception...\n");
    testOut.increaceIndent();

    bool test_passed = false;
    try {
        // Test body here...
        try {
            throw emdevif::ErrorWithCodeException(emdevif::ErrorCode::InvalidArgument, "Test error with code");
        }
        catch (const emdevif::ErrorWithCodeException& e) {
            testOut | [&] {
                std::printf("Caught ErrorWithCode exception: code=%d, message=%s",
                            e.getErrorCode().toUnderlying(),
                            e.what());
            };

            using namespace std::string_view_literals;
            if (e.getErrorCode() != emdevif::ErrorCode::InvalidArgument ||
                std::string_view(e.what()) != "Test error with code"sv) {
                testOut | [] { std::printf("FAILED: Exception data does not match expected values"); };
                throw std::logic_error("Test failed");
            }
        }

        try {
            throw emdevif::ErrorWithCodeException(emdevif::ErrorCode::OutOfMemory);
        }
        catch (const emdevif::ErrorWithCodeException& e) {
            testOut | [&] {
                std::printf("Caught ErrorWithCode exception: code=%d, message=%s",
                            e.getErrorCode().toUnderlying(),
                            e.what());
            };

            using namespace std::string_view_literals;
            if (e.getErrorCode() != emdevif::ErrorCode::OutOfMemory || std::string_view(e.what()) != ""sv) {
                testOut | [] { std::printf("FAILED: Exception data does not match expected values"); };
                throw std::logic_error("Test failed");
            }
        }

        test_passed = true;
        testOut.increaceIndent();
        testOut | [&] { std::printf("PASSED"); };
        testOut.decreaceIndent();
    }
    catch (std::logic_error& e) {
        testOut | [&] { std::printf("FAILED: %s", e.what()); };
    }
    catch (emdevif::ErrorWithCodeException& e) {
        testOut | [&] {
            std::printf("FAILED: Caught unexpected `ErrorWithCode` exception: code=%d, message=%s",
                        e.getErrorCode().toUnderlying(),
                        e.what());
        };
    }
    catch (std::exception& e) {
        testOut | [&] { std::printf("FAILED: Caught unexpected exception: %s", e.what()); };
    }
    catch (...) {
        testOut | [] { std::printf("FAILED: Caught unknown exception"); };
    }

    testOut.decreaceIndent();

    return test_passed ? 0 : 1;
}

#endif  // __cpp_exceptions

}  // namespace

int main()
{
    int failed_tests = 0;

    failed_tests += testTerminate();
    failed_tests += testFatalHandler();
    failed_tests += testAssertFailedHandler();
#ifdef __cpp_exceptions
    failed_tests += testErrorWithCodeException();
#else
    testOut | [] { std::printf("Skipping exception tests because exceptions are disabled"); };
#endif

    std::putchar('\n');
    testOut | [&] {
        if (failed_tests == 0) {
            std::printf("All tests passed");
        }
        else {
            std::printf("%d test(s) failed", failed_tests);
        }
    };
    return failed_tests;
}
