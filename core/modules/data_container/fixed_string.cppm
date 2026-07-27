/**
 * @file fixed_string.cppm
 * @brief 编译期固定大小字符串模块接口
 */

module;

#include "emdevif/core/data_container/fixed_string.hpp"

export module emdevif.core.data_container.fixed_string;

export namespace emdevif {
using ::emdevif::BasicFixedString;
using ::emdevif::FixedString;
using ::emdevif::FixedU16String;
using ::emdevif::FixedU32String;
using ::emdevif::FixedU8String;
using ::emdevif::FixedWString;
using ::emdevif::swap;
}  // namespace emdevif
