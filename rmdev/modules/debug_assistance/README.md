# debug_assistance

调试辅助模块（例如与上位机调试工具数据交互）。

## 依赖

- `emdevif_core`

## 功能

- 提供调试数据输出相关接口
- 便于在开发阶段观测控制量/状态量

## 使用

链接 `rmdev_debug_assistance`（或聚合库 `rmdev`）后，按接口将目标数据打包输出。

## 说明

该模块无独立 CMake 配置项。
