# rmdev

rmdev: RoboMaster Development， RoboMaster 电控开发库。

## 特点

- C++20 模块 + 头文件两种导入方式
- 可按模块/驱动启用，减少无用编译
- 与 `emdevif` 深度协同，适配嵌入式开发场景

## 依赖

- CMake >= 3.28
- 支持 C++20 的编译器
- `emdevif`（必须先加入工程）

## 编码规范

本项目遵循统一编码规范（覆盖 emdevif 与 rmdev 全部子模块）：
- 集成环境内：[`../docs/coding-style.md`](../docs/coding-style.md)（相对本仓库根目录）
- 独立查看：https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/docs/coding-style.md

## 模块组成

模块

* `math` 数学库
* `control_algorithm` 控制算法
* `kinematic_solution` 运动学解算
* `device_model` 设备模型
* `ins` 姿态解算
* `message_manager` 发布/订阅消息管理
* `debug_assistance` 调试辅助

驱动

* `BMI088` BMI088 驱动
* `DJIMotor` 大疆电机驱动

## 使用方法

首先，请参考 [emdevif](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif.git) 的文档，配置 emdevif 库。

完成后，文件结构将会变成：

```
project_root
├── inc/
│   └── ...
├── src/
│   └── ...
├── CMakeLists.txt
├── emdevif_collection/
│   └── emdevif/
│       └── ...
├── rmdev/
│   └── ...
└── ...
```

直接将 rmdev 作为子目录加入工程后执行 `add_subdirectory(rmdev)`。配置变量如下：

| CMake 缓存变量                | 类型     | 默认值  | 说明                    |
|---------------------------|--------|------|-----------------------|
| RMDEV_ENABLE_INS_MODULE   | Bool   | OFF  | 是否使用姿态解算模块            |
| RMDEV_ENABLED_DRIVER_LIST | String | `""` | 要使用的驱动列表。驱动名称之间使用分号分隔 |

说明：由于姿态解算库直接依赖 CMSISDSP，而驱动库往往依赖项较多，因此这两个模块设置变量用于开关。

## 测试

单元测试已迁移至模拟测试环境仓库 `rmdev-dev` 的 `tests/mock/` 目录，使用 GoogleTest 框架。详见 `rmdev-dev` 根目录 `AGENTS.md` 的模拟单元测试章节。
