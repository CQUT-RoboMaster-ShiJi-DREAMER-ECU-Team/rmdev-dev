# AGENTS.md

## 仓库定位
该仓库是 `rmdev` 与 `emdevif` 的开发/测试集成环境，不是最终产品工程模板。

## 主要职责
- 组织跨平台（STM32/ESP32）构建与联调。
- 汇总子模块并提供板级测试入口。
- 维护 `RunningPlatforms/` 与 `TestImplement/` 的一致性。

## 修改约束
- 优先在对应子模块中修复库逻辑；本仓库只做集成与验证层调整。
- 不修改 `Packages/` 和 `emdevif_collection/emdevif/core/depends/` 第三方代码。
- 与平台无关的测试逻辑放在 `TestImplement/`，平台相关逻辑放在 `RunningPlatforms/`。

## 验证建议
- 使用 `PLATFORM_NAME` 与 `BOARD_NAME` 选择目标平台进行构建。
- 变更板级逻辑后，至少在对应板级配置完成一次构建验证。
