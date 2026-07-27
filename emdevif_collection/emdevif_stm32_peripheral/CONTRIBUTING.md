# 贡献指南

`emdevif_stm32_peripheral` 是一个**独立维护**的库（`emdevif` 的 STM32 外设扩展），它的开发 / 测试集成环境是 [`rmdev-dev`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev)。

## 开发模型

`emdevif_stm32_peripheral` 通过 git subtree 集成在 `rmdev-dev` 仓库的 `emdevif_collection/emdevif_stm32_peripheral/` 子目录下，所有开发统一在 `rmdev-dev` 进行并通过 Pull Request 合并。本仓库（`emdevif_stm32_peripheral.git`）已关闭 Pull Request（仍保留 Issue），作为发布镜像由维护者通过 `git subtree push --prefix=emdevif_collection/emdevif_stm32_peripheral ...` 同步回去。

## 在 `rmdev-dev` 中开发

- **库逻辑改动**：直接在 `rmdev-dev` 的 `emdevif_collection/emdevif_stm32_peripheral/` 子目录下修改，无需切换到本仓库。
- **集成验证**：在 `rmdev-dev` 运行模拟单元测试，见 `rmdev-dev` 根 `README.md` 的「快速开始」章节。

完整贡献流程（Fork → 改动 → 测试 → 提交 Pull Request → Review / CI 合并）见 `rmdev-dev` 仓库根目录 [`CONTRIBUTING.md`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/CONTRIBUTING.md) 的「开发与发布流程」章节。合并即完成贡献，无需在子仓库单独提交或同步。

## 反馈与 Issue

Issue 仍可在本仓库（`emdevif_stm32_peripheral.git`）的 Issue 区提交，用于反馈问题或提出功能请求；但代码贡献请统一在 `rmdev-dev` 完成。

## 编码规范

统一编码规范位于 `rmdev-dev` 仓库的 `docs/coding-style.md`：

- 集成环境内（在 `rmdev-dev` 作为子目录，git subtree 集成）：`../../docs/coding-style.md`（相对本仓库根目录）
- 独立查看：https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/docs/coding-style.md

本模块额外约束见 [`AGENTS.md`](AGENTS.md)（HAL/LL 适配与驱动选择路径）。
