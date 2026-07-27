/**
 * @file error_code.hpp
 * @brief 错误码
 */

#pragma once
#ifndef EMDEVIF_CORE_ERROR_HANDLER_ERROR_CODE_HPP
#define EMDEVIF_CORE_ERROR_HANDLER_ERROR_CODE_HPP

#include <cstdint>

#include <compare>
#include <concepts>
#include <exception>
#include <source_location>
#include <utility>

#include "emdevif/core/concepts.hpp"
#include "emdevif/core/error_handler/fatal_handler.hpp"
#include "emdevif/core/simplify_decl_macros.hpp"

namespace emdevif {

/**
 * 错误码
 */
class ErrorCode
{
public:
    /// 错误码的值
    enum ErrorCodeValue : std::int_fast8_t {
        Success = 0,            ///< 成功
        UnknownError = -1,      ///< 未知错误
        InvalidArgument = -2,   ///< 无效参数
        OutOfMemory = -3,       ///< 内存不足
        NotFound = -4,          ///< 未找到
        PermissionDenied = -5,  ///< 权限被拒绝
        Timeout = -6,           ///< 超时
        Full = -7,              ///< 已满
        Empty = -8,             ///< 为空
        OperationFail = -9,     ///< 操作失败
        AlreadyExists = -10,    ///< 已存在
        NotImplemented = -11,   ///< 未实现
        InternalError = -12     ///< 内部错误
    };

private:
    ErrorCodeValue value_{};  ///< 内部存储的错误值

public:
    constexpr ErrorCode() noexcept = default;
    ~ErrorCode() noexcept = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr ErrorCode(const ErrorCodeValue value) noexcept : value_(value) {}  // NOLINT(*-explicit-constructor)

    template<std::integral IntType>
    explicit constexpr ErrorCode(const IntType& value) noexcept : value_(value)
    {
    }

    template<std::integral IntType>
    explicit constexpr operator IntType() const noexcept
    {
        return value_;
    }

    constexpr std::strong_ordering operator<=>(const ErrorCode&) const noexcept = default;

    /**
     * 转化为底层类型
     *
     * 等价于 `static_cast<std::underlying_type_t<ErrorCodeValue>>(value_)`
     * @return 底层类型的值
     */
    [[nodiscard]] constexpr decltype(auto) toUnderlying() const noexcept
    {
        return static_cast<std::underlying_type_t<ErrorCodeValue>>(value_);
    }

    /**
     * 如果未成功（值不是 ErrorCode::Success），终止程序
     * @param sl source_location，默认值为 std::source_location::current()
     */
    void terminateIfNotSucceed(const std::source_location sl = std::source_location::current()) const noexcept
    {
        if (value_ != Success) {
            fatalHandler(sl.file_name(), sl.line(), "Error happened with error code is %d.", static_cast<int>(value_));
        }
    }
};

/**
 * 如果未成功（值不是 ErrorCode::Success），终止程序
 * @param error_code 错误码
 * @param sl source_location，默认值为 std::source_location::current()
 */
inline void terminateIfNotSucceed(const ErrorCode& error_code,
                                  const std::source_location sl = std::source_location::current()) noexcept
{
    error_code.terminateIfNotSucceed(sl);
}

/**
 * 如果是空指针，终止程序
 * @tparam Pointer 指针类型
 * @param ptr 指针的值
 * @param sl source_location，默认值为 std::source_location::current()
 */
template<emdevif::PointerType Pointer>
void terminateIfNullptr(Pointer ptr, const std::source_location sl = std::source_location::current()) noexcept
{
    if (ptr == nullptr) {
        fatalHandler(sl.file_name(), sl.line(), "Null pointer provided.");
    }
}

/**
 * 返回错误码异常
 *
 * 尽管嵌入式开发一般会关闭异常，但 emdevif
 * 仍然提供一种统一的方式将错误码转化为异常的处理方式以便于启用了异常的情况。
 * 该异常的构造必须指定错误码，而异常信息可选，这样在捕获异常时可以获得错误码。
 */
class ErrorWithCodeException : public std::exception
{
public:
    ErrorWithCodeException() = delete;

    ~ErrorWithCodeException() noexcept override = default;

private:
    ErrorCode ec_;           ///< 错误码
    const char* error_str_;  ///< 错误信息

public:
    [[nodiscard]] const char* what() const noexcept override
    {
        return error_str_;
    }

    /// 获取错误码
    [[nodiscard]] ErrorCode getErrorCode() const noexcept
    {
        return ec_;
    }

    /**
     * 构造返回错误码异常
     * @attention 当且仅当编译器未关闭异常的情况才能使用。
     * @param error_code 错误码
     * @param message 异常消息，默认为空字符串。
     * 注意，该字符串需要确保在异常对象的生命周期内有效，建议使用字符串字面量或全局静态字符串。
     */
    explicit ErrorWithCodeException(const ErrorCode error_code, const char* message = "") noexcept
#ifdef __cpp_exceptions
        : ec_(error_code), error_str_(message){}
#else
        = EMDEVIF_REASON_DELETE("The compiler disabled C++ exceptions.");
#endif
};

}  // namespace emdevif

#endif  // !EMDEVIF_CORE_ERROR_HANDLER_ERROR_CODE_HPP
