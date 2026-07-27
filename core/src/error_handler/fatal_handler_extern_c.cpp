/**
 * @file fatal_handler_extern_c.cpp
 * @brief 致命错误 - C 语言 API
 */

#include <cstdarg>

#include "emdevif/core/detail/config.hpp"

#define EMDEVIF_CORE_ERROR_HANDLER_IN_C_API_WRAPPER_SRC_
#include "emdevif/core/attributes_and_useful_macros.h"
#include "emdevif/core/fatal_handler.h"

#if (defined(EMDEVIF_USE_MODULES) && EMDEVIF_USE_MODULES)
import emdevif.core.error_handler;
#else
#include "emdevif/core/error_handler.hpp"
#endif

EMDEVIF_EXTERN_C_BEGIN

/**
 * 供 C 语言调用的致命错误处理函数
 * @attention 不应该直接调用这个函数，而是应该使用宏 @ref EMDEVIF_FATAL_HANDLER
 *
 * @param file 所在文件名
 * @param line 所在行号
 * @param format 错误信息的格式化字符串
 * @param ... 填充占位符的值
 */
EMDEVIF_NO_RETURN void emdevif_fatal_handler_(const char* file, const int line, const char* format, ...)
{
    std::va_list args;
    va_start(args, format);
    emdevif::fatalHandler(file, line, format, args);
    va_end(args);
}

/**
 * 供 C 语言调用的断言函数
 * @attention 不应该直接调用这个函数，而是应该使用宏 @ref EMDEVIF_ASSERT
 * @param param 参数
 */
void emdevif_assert_(emdevif_AssertFuncParam_ param)
{
    if (param.message == nullptr) {
        param.message = "";
    }
    emdevif::emdevif_assert(param.condition,
                            param.file,
                            param.line,
                            param.func_name,
                            param.condition_name,
                            param.message);
}

EMDEVIF_EXTERN_C_END
