/**
 * @file PeripheralHandleMap.cppm
 * @brief 外设句柄映射 C++20 模块封装，重导出 peripheral_handle_map.hpp 中的接口
 */

module;

#include "emdevif/peripheral/peripheral_handle_map.hpp"

export module emdevif.peripheral.peripheral_handle_map;

export namespace emdevif {
using ::emdevif::PeripheralHandleMap;
}
