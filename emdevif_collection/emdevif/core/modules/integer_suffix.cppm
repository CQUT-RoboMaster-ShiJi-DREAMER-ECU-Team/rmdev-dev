/**
 * @file integer_suffix.cppm
 * @brief 整型字面量后缀
 */

module;

#include "emdevif/core/integer_suffix.hpp"

export module emdevif.core.integer_suffix;

export namespace emdevif::inline literals::inline integer_literals {
using ::emdevif::operator""_u8;
using ::emdevif::operator""_u16;
using ::emdevif::operator""_u32;
using ::emdevif::operator""_u64;
using ::emdevif::operator""_i8;
using ::emdevif::operator""_i16;
using ::emdevif::operator""_i32;
using ::emdevif::operator""_i64;
using ::emdevif::operator""_ptrdiff;
using ::emdevif::operator""_zu;
}  // namespace emdevif::inline literals::inline integer_literals
