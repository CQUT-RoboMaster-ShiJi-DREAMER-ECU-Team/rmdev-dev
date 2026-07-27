/**
 * @file peripheral_handle_map.hpp
 * @brief 外设句柄映射查找工具，提供编译期外设句柄查表功能
 */

#pragma once
#ifndef EMDEVIF_PERIPHERAL_PERIPHERAL_HANDLE_MAP_HPP
#define EMDEVIF_PERIPHERAL_PERIPHERAL_HANDLE_MAP_HPP

#include <string_view>

#include "emdevif/core/simplify_decl_macros.hpp"

namespace emdevif {

namespace user_impl::peripheral_handle_map {
void* findHandle(std::string_view name) noexcept;
}

/// @brief 外设句柄映射查找工具类，通过用户提供的 findHandle 函数根据名称查找外设句柄
class PeripheralHandleMap
{
public:
    PeripheralHandleMap() = delete;
    ~PeripheralHandleMap() = delete;
    EMDEVIF_DELETE_COPY_AND_MOVE(PeripheralHandleMap);

    /**
     * @brief 根据名称查找外设句柄
     * @param name 外设句柄名称
     * @return 外设句柄指针，未找到时返回 nullptr
     */
    static void* findHandle(const std::string_view name) noexcept
    {
        return user_impl::peripheral_handle_map::findHandle(name);
    }
};

}  // namespace emdevif

#endif  // !EMDEVIF_PERIPHERAL_PERIPHERAL_HANDLE_MAP_HPP
