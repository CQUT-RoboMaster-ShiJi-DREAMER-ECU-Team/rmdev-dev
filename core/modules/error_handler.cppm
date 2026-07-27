/**
 * @file error_handler.cppm
 * @brief 错误处理
 */

module;

#include "emdevif/core/error_handler.hpp"

export module emdevif.core.error_handler;

export namespace emdevif {
using ::emdevif::AssertFailedHandler;
using ::emdevif::emdevif_assert;
using ::emdevif::ErrorCode;
using ::emdevif::ErrorWithCodeException;
using ::emdevif::fatalHandler;
using ::emdevif::FatalHandlerCallBack;
using ::emdevif::registerAssertFailedHandler;
using ::emdevif::registerFatalHandler;
using ::emdevif::registerTerminateFunction;
using ::emdevif::terminate;
using ::emdevif::TerminateFunction;
using ::emdevif::terminateIfNotSucceed;
using ::emdevif::terminateIfNullptr;
}  // namespace emdevif
