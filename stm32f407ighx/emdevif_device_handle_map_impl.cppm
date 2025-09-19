/**
 * @file emdevif_device_handle_map_impl.cppm
 * @author DuYicheng
 * @date 2025-09-04
 * @brief 实现当前平台的设备句柄键值对
 */

module;

#include <string_view>

#include "usart.h"

export module emdevif.userDeclares;

export import emdevif.container.map;

export namespace emdevif::user_declares {

constexpr auto peripheral_handle_map = makeStaticMap<std::string_view, void*>({{"test transmit serial", &huart6}});

}  // namespace emdevif::user_declares
