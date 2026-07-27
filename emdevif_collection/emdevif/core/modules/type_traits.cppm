/**
 * @file type_traits.cppm
 * @brief 类型特征（STL type_traits 头文件的扩展）
 */

module;

#include <type_traits>
#include <utility>

#include "emdevif/core/type_traits.hpp"

export module emdevif.core.type_traits;

export namespace emdevif {
using ::emdevif::is_consteval;
}  // namespace emdevif
