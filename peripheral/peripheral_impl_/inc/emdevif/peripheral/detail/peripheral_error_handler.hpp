/**
 * @file peripheral_error_handler.hpp
 * @brief 外设错误处理工具类，提供外设实例校验功能
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_DETAIL_PERIPHERAL_ERROR_HANDLER_HPP
#define EMDEVIF_PERIPHERAL_DETAIL_PERIPHERAL_ERROR_HANDLER_HPP

#include <string_view>
#include <type_traits>

#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/core/simplify_decl_macros.hpp"

namespace emdevif::detail {

/// @brief 外设错误处理工具类，提供外设实例是否存在等校验功能
class PeripheralErrorHandler
{
public:
    EMDEVIF_DECLARE_ZERO_INSTANCE(PeripheralErrorHandler);

    /**
     * @brief 检查外设实例句柄是否存在，不存在则触发错误处理
     * @param handle 外设实例句柄指针
     * @param peripheral_name 外设类型名称（如 "CAN"、"GPIO"）
     * @param handle_name 外设句柄名称
     */
    static constexpr void checkInstanceIsExist(const void* handle,
                                               const std::string_view peripheral_name,
                                               const std::string_view handle_name) noexcept
    {
        if (handle == nullptr) {
            if (std::is_constant_evaluated()) {
                // 通过 ill-formated code（在常量表达式求值上下文中调用非常量表达式函数）触发编译期错误信息
                ThisIsACompileTimeMessage_CouldNotFoundHandle();
                return;
            }

            EMDEVIF_FATAL_HANDLER("Could not find the %s handle named \"%s\"",
                                  peripheral_name.data(),
                                  handle_name.data());
        }
    }

private:
    static void ThisIsACompileTimeMessage_CouldNotFoundHandle() noexcept {}
};

}  // namespace emdevif::detail

#endif  // !EMDEVIF_PERIPHERAL_DETAIL_PERIPHERAL_ERROR_HANDLER_HPP
