# AGENTS.md

## 模块定位

`rmdev` 是 RoboMaster 电控开发库，统一组织算法、模型与驱动模块，并依赖 `emdevif` 提供底层抽象。

## 仓库结构

```
rmdev/
├── modules/                    # 功能模块
│   ├── math/                   #   数学库
│   ├── control_algorithm/      #   控制算法
│   ├── kinematic_solution/     #   运动学解算
│   ├── device_model/           #   设备模型
│   ├── ins/                    #   姿态解算（需 CMSIS-DSP）
│   ├── message_manager/        #   发布订阅消息管理
│   └── debug_assistance/       #   调试辅助
├── drivers/                    # 驱动模块
│   ├── BMI088/                 #   BMI088 IMU 驱动
│   └── DJIMotor/               #   大疆电机驱动
└── OpenOCD-BoardConfigFileTemplates/  # OpenOCD 板级配置模板
```

### 集成方式

将本仓库作为子目录加入工程后执行 `add_subdirectory(rmdev)`。此时：
- **基础模块**（math / control_algorithm / kinematic_solution / device_model / debug_assistance / message_manager）总是编译并链接
- **INS 模块**：通过 `RMDEV_ENABLE_INS_MODULE=ON` 启用，依赖 CMSIS-DSP
- **驱动模块**：通过 `RMDEV_ENABLED_DRIVER_LIST` 按需启用

## 构建系统

### 关键 CMake 变量

| 变量 | 默认 | 说明 |
|---|---|---|
| `RMDEV_ENABLE_INS_MODULE` | `OFF` | INS 姿态解算模块开关，需 CMSIS-DSP |
| `RMDEV_ENABLED_DRIVER_LIST` | `""` | 启用的驱动列表（分号分隔），如 `"BMI088;DJIMotor"` |

### 依赖链

- `rmdev` → `emdevif`（必须先 `add_subdirectory`）
- `rmdev_ins` → CMSIS-DSP（由 CubeMX 或用户提供）

## 参考资料

- 完整使用指南：`README.md`
