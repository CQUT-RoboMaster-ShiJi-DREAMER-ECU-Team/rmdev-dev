# DJIMotor

大疆电机驱动模块。

## 依赖

- `emdevif_core`
- `emdevif_peripheral`
- `rmdev_device_model`
- `mp-units`（用于单位安全表达）

## 启用方式

在上层 `rmdev` 中通过驱动列表启用：

```cmake
set(RMDEV_ENABLED_DRIVER_LIST "DJIMotor" CACHE INTERNAL "" FORCE)
# 也可与其他驱动组合，例如："BMI088;DJIMotor"
```

## 功能说明

- 提供 DJI 电机通讯与模型交互接口
- 使用 `mp-units` 进行单位安全表达（电流、角度、转速等）

## 使用流程

1. 在上层 `rmdev` 中通过 `RMDEV_ENABLED_DRIVER_LIST` 启用本驱动。
2. 先完成 CAN / 串口外设句柄映射（通过链接期注入
   `emdevif::user_impl::peripheral_handle_map::findHandle`，详见
   [`emdevif/docs/peripheral.md`](../../../emdevif_collection/emdevif/docs/peripheral.md)）。
3. 与控制算法层之间通过 `rmdev_device_model` 交换状态与目标值。
