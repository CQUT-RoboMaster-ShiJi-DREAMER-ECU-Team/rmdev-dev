/**
 * @file class_dji_motor.hpp
 * @brief
 */

#pragma once
#ifndef RMDEV_DRIVER_DJI_MOTOR_CLASS_DJI_MOTOR_HPP
#define RMDEV_DRIVER_DJI_MOTOR_CLASS_DJI_MOTOR_HPP

namespace rmdev {

/// @brief 大疆电机抽象类（GM6020 / C620 / C610）
class DJIMotor
{
public:
    using SendDataType = int16_t;

    using OriginEcdAngleType = uint16_t;
    using EcdAngleType = int16_t;

    /// @brief 电机反馈信息数据结构体
    struct MotorInfo {
        /// 编码器角度值
        /// @note 范围：0 ~ 8191(对应转子机械角度为 0~360°)
        OriginEcdAngleType ecd_angle;
        int16_t rotor_speed_rpm;  ///< 转速（单位：rpm）
        int16_t torque_current;   ///< 实际转矩电流
        uint8_t temperature;      ///< 温度
    };

    /// @brief 电机控制模式
    /// @note 仅有 GM6020 支持电压控制模式
    enum class CtrlMode : uint_fast8_t {
        Voltage = 0,
        Current
    };

    /// @brief 电机型号枚举
    enum class Type : uint_fast8_t {
        GM6020 = 0,
        C620,
        C610
    };

private:
    Type type_;                        ///< 电机型号

    CtrlMode ctrl_mode_;               ///< 电机控制模式

    OriginEcdAngleType zero_ecd_pos_;  ///< 零度的编码器位置

    MotorInfo motor_info_{};           ///< 电机反馈信息

    SendDataType send_target_{};       ///< 待发送的控制目标值

    MotorRotateRecorder<uint16_t> rotate_recorder_{{
        .mid_pos = 4096,
        .high_pos = 5000,
        .low_pos = 3500,
        .now_pos = 4095,
        .previous_pos = 4095,
    }};  ///< 电机多圈记录器

public:
    DJIMotor() = delete;

    /**
     * @brief 构造大疆电机对象
     * @param type 电机型号
     * @param mode 控制模式
     * @param zero_ecd_pos 编码器零位（默认 4096）
     */
    constexpr DJIMotor(const Type type, const CtrlMode mode, const OriginEcdAngleType zero_ecd_pos = 4096) noexcept
        : type_(type), ctrl_mode_(mode), zero_ecd_pos_(zero_ecd_pos)
    {
        if (type_ != Type::GM6020) {
            EMDEVIF_ASSERT(mode == CtrlMode::Current, "Only GM6020 supports voltage control");
        }
    }

    /**
     * @brief 从 CAN 接收数据解析电机反馈信息
     * @param rx_data CAN 接收到的 7 字节数据
     */
    void receiveInfoFrom(const std::span<const uint8_t, 7> rx_data) noexcept
    {
        // NOLINTBEGIN(*-narrowing-conversions)
        motor_info_.ecd_angle = ((rx_data[0] << 8) | rx_data[1]);
        motor_info_.rotor_speed_rpm = ((rx_data[2] << 8) | rx_data[3]);
        motor_info_.torque_current = ((rx_data[4] << 8) | rx_data[5]);
        motor_info_.temperature = rx_data[6];
        // NOLINTEND(*-narrowing-conversions)
    }

    /**
     * @brief 获取电机型号
     * @return 电机型号
     */
    [[nodiscard]] Type getType() const noexcept
    {
        return type_;
    }

    /**
     * @brief 获取电机控制模式
     * @return 控制模式
     */
    [[nodiscard]] CtrlMode getCtrlMode() const noexcept
    {
        return ctrl_mode_;
    }

    /**
     * @brief 获取编码器零位
     * @return 编码器零位值
     */
    [[nodiscard]] OriginEcdAngleType getZeroPositionEcd() const noexcept
    {
        return zero_ecd_pos_;
    }

    /**
     * @brief 获取转子旋转圈数
     * @return 旋转圈数
     */
    [[nodiscard]] int_fast32_t getRotorCount() noexcept
    {
        return rotate_recorder_.getRotateCount();
    }

    /**
     * @brief 记录转子旋转（更新多圈计数）
     */
    void recordRotate() noexcept
    {
        rotate_recorder_.record(motor_info_.ecd_angle);
    }

    /**
     * @brief 获取待发送的控制目标值
     * @return 控制目标值
     */
    [[nodiscard]] SendDataType getSendTarget_() const noexcept
    {
        return send_target_;
    }

    /**
     * @brief 设置电流目标值
     * @tparam CalType 计算数据类型
     * @param current 电流目标值
     */
    template<typename CalType>
    void setCurrent(mp_units::quantity<mp_units::si::ampere, CalType> current) noexcept
    {
        using namespace mp_units;
        using namespace mp_units::si::unit_symbols;

        EMDEVIF_ASSERT(ctrl_mode_ == CtrlMode::Current, "The motor control mode was not 'CtrlMode::Current'");

        switch (type_) {
        case Type::C610: {
            current = std::clamp<decltype(current)>(current, -10 * A, 10 * A);

            const CalType pure_value = current.numerical_value_in(si::ampere);
            send_target_ = static_cast<SendDataType>(pure_value * CalType(10000) / CalType(10));
            send_target_ = std::clamp<SendDataType>(send_target_, -10000, 10000);
        } break;
        case Type::GM6020: {
            current = std::clamp<decltype(current)>(current, -3 * A, 3 * A);

            const CalType pure_value = current.numerical_value_in(si::ampere);
            send_target_ = static_cast<SendDataType>(pure_value * CalType(16384) / CalType(3));
            send_target_ = std::clamp<SendDataType>(send_target_, -16384, 16384);
        } break;
        case Type::C620: {
            current = std::clamp<decltype(current)>(current, -20 * A, 20 * A);

            const CalType pure_value = current.numerical_value_in(si::ampere);
            send_target_ = static_cast<SendDataType>(pure_value * CalType(16384) / CalType(20));
            send_target_ = std::clamp<SendDataType>(send_target_, -16384, 16384);
        } break;
        default:
            EMDEVIF_ASSERT(false, "Program should not touch here");
        }
    }

    /**
     * @brief 设置电压目标值
     * @param voltage 电压目标值
     */
    void setVoltage(const SendDataType voltage) noexcept
    {
        EMDEVIF_ASSERT(ctrl_mode_ == CtrlMode::Voltage, "The motor control mode was not 'CtrlMode::Voltage'");

        if (type_ == Type::GM6020) {
            send_target_ = std::clamp<SendDataType>(voltage, -25000, 25000);
        }
        else {
            EMDEVIF_ASSERT(false, "The motor type 'Type::C620' and 'Type::C610' does not support voltage control mode");
        }
    }

    /**
     * @brief 获取原始电机反馈信息
     * @return 电机反馈信息的常量引用
     */
    [[nodiscard]] const MotorInfo& getPureMotorInfo() const noexcept
    {
        return motor_info_;
    }

    /**
     * @brief 获取原始编码器角度值
     * @return 编码器角度值（0 ~ 8191）
     */
    [[nodiscard]] OriginEcdAngleType getOriginEcdAngle() const noexcept
    {
        return motor_info_.ecd_angle;
    }

    /**
     * @brief 获取减去零位后的编码器角度值
     * @return 相对编码器角度
     */
    [[nodiscard]] EcdAngleType getEcdAngle() const noexcept
    {
        const auto signed_ecd_value = static_cast<EcdAngleType>(motor_info_.ecd_angle);
        const auto signed_zero_ecd_value = static_cast<EcdAngleType>(zero_ecd_pos_);
        return signed_ecd_value - signed_zero_ecd_value;
    }

    /**
     * @brief 获取电机角度（度）
     * @tparam CalType 计算数据类型
     * @return 角度（度）
     */
    template<typename CalType = float>
    [[nodiscard]] mp_units::quantity<mp_units::si::degree, CalType> getPosDegree() const noexcept
    {
        using namespace mp_units;

        const auto ecd_angle = static_cast<CalType>(getEcdAngle());
        return CalType(ecd_angle * CalType(360) / CalType(8191)) * si::degree;
    }

    /**
     * @brief 获取电机位置（弧度）
     * @tparam CalType 计算数据类型
     * @return 电机位置（弧度）
     */
    template<typename CalType = float>
    [[nodiscard]] mp_units::quantity<mp_units::si::radian, CalType> getPosRadian() const noexcept
    {
        using namespace mp_units;

        const auto ecd_angle = static_cast<CalType>(getEcdAngle());
        return CalType(ecd_angle * CalType(2) * CalType(std::numbers::pi) / CalType(8191)) * si::radian;
    }

    /**
     * @brief 获取电机转速（rpm）
     * @tparam CalType 计算数据类型
     * @return 电机转速（rpm）
     */
    template<typename CalType = float>
    [[nodiscard]] mp_units::quantity<rpm, CalType> getSpeed() const noexcept
    {
        using namespace mp_units;
        return CalType(motor_info_.rotor_speed_rpm) * rpm;
    }

    emdevif::ErrorCode send() = delete;
};

template<typename T>
struct IsDJIMotor : public std::is_same<DJIMotor, T> {
};

template<typename T>
constexpr bool IsDJIMotor_v = IsDJIMotor<T>::value;

template<typename T>
concept ValidDJIMotor = IsDJIMotor_v<T>;

}  // namespace rmdev

#endif  // !RMDEV_DRIVER_DJI_MOTOR_CLASS_DJI_MOTOR_HPP
