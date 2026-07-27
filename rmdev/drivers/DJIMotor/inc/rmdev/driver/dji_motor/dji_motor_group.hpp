/**
 * @file dji_motor_group.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_DRIVER_DJI_MOTOR_DJI_MOTOR_GROUP_HPP
#define RMDEV_DRIVER_DJI_MOTOR_DJI_MOTOR_GROUP_HPP

#include "emdevif/core/detail/config.hpp"

#include "rmdev/driver/dji_motor/class_dji_motor.hpp"
#include "rmdev/driver/dji_motor/can_address.hpp"

namespace rmdev {

/// @brief 大疆电机组的构建器（聚合类型）
class DJIMotorGroupBuilder
{
public:
    /// @brief 电机 ID 分组
    enum class IdGroup : uint_fast8_t {
        id1234 = 0,
        id5678,
        id567
    };

    emdevif::Can can;     ///< CAN 外设
    DJIMotor::Type type;  ///< 电机型号
    IdGroup id_group;     ///< ID 分组

    /**
     * @brief 根据电机型号和 ID 分组生成 CAN 发送报文标识符
     * @return CAN 发送报文标识符
     */
    [[nodiscard]] constexpr DJIMotorTransmitCanBase phraseCanBase() const noexcept
    {
        switch (type) {
        case DJIMotor::Type::GM6020:
            if (id_group == IdGroup::id1234) {
                return DJIMotorTransmitCanBase::GM6020_TX1234;
            }
            if (id_group == IdGroup::id567) {
                return DJIMotorTransmitCanBase::GM6020_TX567;
            }
            EMDEVIF_FATAL_HANDLER("Invalid ID group for GM6020");

        case DJIMotor::Type::C620:
            if (id_group == IdGroup::id1234) {
                return DJIMotorTransmitCanBase::C620_TX1234;
            }
            if (id_group == IdGroup::id5678) {
                return DJIMotorTransmitCanBase::C620_TX5678;
            }
            EMDEVIF_FATAL_HANDLER("Invalid ID group for C620");

        case DJIMotor::Type::C610:
            if (id_group == IdGroup::id1234) {
                return DJIMotorTransmitCanBase::C610_TX1234;
            }
            if (id_group == IdGroup::id5678) {
                return DJIMotorTransmitCanBase::C610_TX5678;
            }
            EMDEVIF_FATAL_HANDLER("Invalid ID group for C610");

        default:
            EMDEVIF_FATAL_HANDLER("Invalid type");
        }
    }
};
static_assert(std::is_aggregate_v<DJIMotorGroupBuilder>);

/// @brief 大疆电机组模板类，管理同组电机的 CAN 通信
template<std::size_t motor_count_ = 1>
class DJIMotorGroup final : public MotorGroup<DJIMotor, motor_count_>
{
private:
    const emdevif::Can can_;                   ///< CAN 外设
    DJIMotorTransmitCanBase can_tx_can_base_;  ///< CAN 发送报文标识符

    using Super_ = MotorGroup<DJIMotor, motor_count_>;

    using DetailType_ = Super_::SendDataType;

public:
    static_assert(motor_count_ > 0 && motor_count_ <= 4, "The counts of motors in this group should be 1~4");

    using MotorList = Super_::MotorList;
    static constexpr std::size_t motor_count = Super_::motor_count;

    DJIMotorGroup() = delete;

private:
    /**
     * @brief 构造大疆电机组
     * @param can_name CAN 总线名称
     * @param can_tx_can_base CAN 发送报文标识符
     * @param motors 电机引用列表
     */
    constexpr DJIMotorGroup(const std::string_view can_name,
                            const DJIMotorTransmitCanBase can_tx_can_base,
                            const MotorList& motors) noexcept
        : Super_(motors), can_(can_name), can_tx_can_base_(can_tx_can_base)
    {
    }

    /**
     * @brief 构造大疆电机组（变参模板）
     * @tparam Motors 电机类型
     * @param can_name CAN 总线名称
     * @param can_tx_can_base CAN 发送报文标识符
     * @param motors 电机引用
     */
    template<typename... Motors>
        requires(sizeof...(Motors) == motor_count && (ValidDJIMotor<std::decay_t<Motors>> && ...))
    constexpr DJIMotorGroup(const std::string_view can_name,
                            const DJIMotorTransmitCanBase can_tx_can_base,
                            Motors&&... motors) noexcept
        : Super_(std::forward<Motors>(motors)...), can_(can_name), can_tx_can_base_(can_tx_can_base)
    {
    }

public:
    /**
     * @brief 通过构建器构造大疆电机组
     * @param builder 电机组构建器
     * @param motors 电机引用列表
     */
    constexpr DJIMotorGroup(const DJIMotorGroupBuilder& builder, const MotorList& motors) noexcept
        : Super_(motors), can_(builder.can), can_tx_can_base_(builder.phraseCanBase())
    {
    }

    /**
     * @brief 通过构建器构造大疆电机组（变参模板）
     * @tparam Motors 电机类型
     * @param builder 电机组构建器
     * @param motors 电机引用
     */
    template<typename... Motors>
        requires(sizeof...(Motors) == motor_count && (ValidDJIMotor<std::decay_t<Motors>> && ...))
    explicit constexpr DJIMotorGroup(const DJIMotorGroupBuilder& builder, Motors&&... motors) noexcept
        : Super_(std::forward<Motors>(motors)...), can_(builder.can), can_tx_can_base_(builder.phraseCanBase())
    {
    }

private:
    /**
     * @brief 发送控制目标值到电机组
     * @param targets 各电机的控制目标值
     * @return 错误码
     */
    [[nodiscard]] emdevif::ErrorCode sendImpl(const std::span<const DetailType_> targets) const noexcept override
    {
        using namespace emdevif;

        const auto tx_data_size = targets.size() * 2;

        const Can::DataHeader tx_header{
            .id = can_tx_can_base_,
            .ide = Can::DataIdentifier::StandardId,
            .rtr = Can::RemoteTransmissionRequest::DataFrame,
            .dlc = tx_data_size,
        };

        uint8_t tx_data[8];
        for (std::size_t tx_index = 0; const auto& target : targets) {
            tx_data[tx_index++] = (target >> 8) & 0xFF;
            tx_data[tx_index++] = target & 0xFF;
        }
        return can_.transmit(false, tx_header, {tx_data, tx_data_size});
    }
};

}  // namespace rmdev

#endif  // !RMDEV_DRIVER_DJI_MOTOR_DJI_MOTOR_GROUP_HPP
