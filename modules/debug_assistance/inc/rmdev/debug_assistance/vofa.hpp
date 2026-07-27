/**
 * @file vofa.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_DEBUG_ASSISTANCE_VOFA_HPP
#define RMDEV_DEBUG_ASSISTANCE_VOFA_HPP

#include <cstdint>

#include <algorithm>
#include <array>
#include <bit>
#include <span>

#include "emdevif/core/endian.hpp"
#include "emdevif/core/error_handler.hpp"

namespace rmdev::debug_assistance::vofa {

/// @brief VOFA+ JustFloat 协议适配类
class JustFloat
{
public:
    /// @brief VOFA+ JustFloat 数据帧尾
    static constexpr std::array<uint8_t, 4> frame_tail{0x00, 0x00, 0x80, 0x7f};

    /**
     * @brief 将浮点数组转换为小端字节序（若本机为大端则进行字节交换）
     * @param data 浮点数组
     * @return 转换后的数组
     */
    static std::span<float> toLittleEndian(const std::span<float> data) noexcept
    {
        static_assert(std::endian::native == std::endian::big || std::endian::native == std::endian::little,
                      "Not support mixed endian.");

        if constexpr (std::endian::native == std::endian::big) {
            for (auto& v : data) {
                v = emdevif::byteSwap(v);
            }
        }

        return data;
    }

    /**
     * @brief 在数据末尾追加 VOFA+ 帧尾
     * @param buffer 浮点缓冲区
     * @param data_count 有效浮点数数量
     * @return 追加帧尾后的字节序列
     */
    static std::span<uint8_t> appendFrameTail(const std::span<float> buffer, const std::size_t data_count) noexcept
    {
        const auto origin_buffer_length = data_count;
        const auto p_end = reinterpret_cast<uint8_t*>(&buffer[data_count]);

        std::ranges::copy(frame_tail.begin(), frame_tail.end(), p_end);

        return {reinterpret_cast<uint8_t*>(buffer.data()), origin_buffer_length * sizeof(float) + frame_tail.size()};
    }

    /**
     * @brief 处理浮点数据：先转为小端字节序，再追加帧尾
     * @param buffer 浮点缓冲区
     * @param data_count 有效浮点数数量
     * @return 处理后的字节序列
     */
    static std::span<uint8_t> processData(const std::span<float> buffer, const std::size_t data_count) noexcept
    {
        toLittleEndian(buffer);
        return appendFrameTail(buffer, data_count);
    }
};

}  // namespace rmdev::debug_assistance::vofa

#endif  // !RMDEV_DEBUG_ASSISTANCE_VOFA_HPP
