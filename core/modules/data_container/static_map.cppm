/**
 * @file static_map.cppm
 * @brief 编译期静态键值对模块接口
 */

module;

#include "emdevif/core/data_container/static_map.hpp"

export module emdevif.core.data_container.static_map;

export namespace emdevif {
using ::emdevif::makeStaticMap;
using ::emdevif::StaticMap;
}  // namespace emdevif
