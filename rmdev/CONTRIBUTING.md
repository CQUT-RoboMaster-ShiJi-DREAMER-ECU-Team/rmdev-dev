# 贡献指南

`rmdev` 是 RoboMaster 电控开发库，统一组织算法、模型与驱动模块。它的开发与测试集成环境是 [`rmdev-dev`](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev)。

## 在哪里开发

`rmdev` 内的所有模块均在本仓库中直接维护。模块逻辑改动无需切换到其他仓库。

集成验证在 `rmdev-dev` 中进行。改动后请同步到 `rmdev-dev` 并运行模拟单元测试，确保集成路径无回归。

## 编码规范

统一编码规范位于 `rmdev-dev` 仓库的 `docs/coding-style.md`：

- 集成环境内：`../docs/coding-style.md`（相对本仓库根目录）
- 独立查看：https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/docs/coding-style.md

裁剪、接口与验证要求见 [`AGENTS.md`](AGENTS.md)。
