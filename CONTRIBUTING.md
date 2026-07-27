# 贡献指南

`emdevif_stm32_peripheral` 是一个**独立维护**的库（`emdevif` 的 STM32 外设扩展），但它的开发 / 测试集成环境是 [`rmdev-dev`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev)。

## 在哪里开发

- **库逻辑改动**：直接在本仓库提交并验证。
- **集成验证**：改动合并后，同步到 `rmdev-dev` 并运行模拟单元测试，确保集成路径无回归。

## 多仓库协作流程

完整流程见 `rmdev-dev` 根目录 [`AGENTS.md`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/AGENTS.md) 的「多仓库协作流程」章节。要点：

1. 库逻辑优先在子模块独立仓库完成；
2. 同步到 `rmdev-dev` 做集成验证；
3. 仅构建脚本、测试框架、文档聚合等集成层改动才在 `rmdev-dev` 提交。

## 编码规范

统一编码规范位于 `rmdev-dev` 仓库的 `docs/coding-style.md`：

- 集成环境内：`../../docs/coding-style.md`（相对本仓库根目录）
- 独立查看：https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/docs/coding-style.md

本模块额外约束见 [`AGENTS.md`](AGENTS.md)（HAL/LL 适配与驱动选择路径）。
