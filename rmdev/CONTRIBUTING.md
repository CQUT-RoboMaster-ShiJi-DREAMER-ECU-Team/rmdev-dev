# 贡献指南

`rmdev` 是 RoboMaster 电控开发库，统一组织算法、模型与驱动模块。它的开发与测试集成环境是 [`rmdev-dev`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev)。

## 开发模型

`rmdev` 通过 git subtree 集成在 `rmdev-dev` 仓库的 `rmdev/` 子目录下，所有开发统一在 `rmdev-dev` 进行并通过 Pull Request 合并。本仓库（`rmdev.git`）已关闭 Pull Request（仍保留 Issue），作为发布镜像由维护者通过 `git subtree push --prefix=rmdev ...` 同步回去。

## 在 `rmdev-dev` 中开发

1. Fork [`rmdev-dev`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev)，在 `rmdev/` 子目录下完成改动；
2. 在 `rmdev-dev` 运行模拟单元测试（见 `rmdev-dev` 根 `README.md` 的「快速开始」章节）确保无回归；
3. 向 `rmdev-dev` 提交 Pull Request，通过 Review / CI 后合并，即完成贡献。

无需在子仓库单独提交或同步——本仓库的更新由维护者统一推送。

## 反馈与 Issue

Issue 仍可在本仓库（`rmdev.git`）的 Issue 区提交，用于反馈问题或提出功能请求；但代码贡献请统一在 `rmdev-dev` 完成。完整流程见 `rmdev-dev` 仓库根目录 [`CONTRIBUTING.md`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/CONTRIBUTING.md) 的「开发与发布流程」章节。

## 编码规范

统一编码规范位于 `rmdev-dev` 仓库的 `docs/coding-style.md`：

- 集成环境内（在 `rmdev-dev` 作为子目录，git subtree 集成）：`../docs/coding-style.md`（相对本仓库根目录）
- 独立查看：https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/docs/coding-style.md

裁剪、接口与验证要求见 [`AGENTS.md`](AGENTS.md)。
