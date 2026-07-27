# 贡献指南

`emdevif` 是一个**独立维护**的库（嵌入式开发接口抽象），它的开发 / 测试集成环境是 [`rmdev-dev`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev)。

## 开发模型

`emdevif` 通过 git subtree 集成在 `rmdev-dev` 仓库的 `emdevif_collection/emdevif/` 子目录下，所有开发统一在 `rmdev-dev` 进行并通过 Pull Request 合并。本仓库（`emdevif.git`）已关闭 Pull Request（仍保留 Issue），作为发布镜像由维护者通过 `git subtree push --prefix=emdevif_collection/emdevif ...` 同步回去。

## 在 `rmdev-dev` 中开发

- **库逻辑改动**：直接在 `rmdev-dev` 的 `emdevif_collection/emdevif/` 子目录下修改，无需切换到本仓库。
- **集成验证**：在 `rmdev-dev` 运行模拟单元测试。`rmdev-dev` 提供了「C++20 模块 开/关」×「异常 开/关」的完整测试预设，见 `rmdev-dev` 根 `README.md` 的「快速开始」章节。

完整贡献流程（Fork → 改动 → 测试 → 提交 Pull Request → Review / CI 合并）见 `rmdev-dev` 仓库根目录 [`CONTRIBUTING.md`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/CONTRIBUTING.md) 的「开发与发布流程」章节。合并即完成贡献，无需在子仓库单独提交或同步。

## 反馈与 Issue

Issue 仍可在本仓库（`emdevif.git`）的 Issue 区提交，用于反馈问题或提出功能请求；但代码贡献请统一在 `rmdev-dev` 完成。

## 编码规范

统一编码规范位于 `rmdev-dev` 仓库的 `docs/coding-style.md`：

- 集成环境内（在 `rmdev-dev` 作为子目录，git subtree 集成）：`../../docs/coding-style.md`（相对本仓库根目录）
- 独立查看：https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/docs/coding-style.md

涵盖命名、格式、Doxygen 注释、工程约束、模块扩展指引与验证要求。
