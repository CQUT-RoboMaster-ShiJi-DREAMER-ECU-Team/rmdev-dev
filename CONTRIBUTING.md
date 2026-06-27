# 贡献指南

`rmdev-dev` 是 `emdevif` 与 `rmdev` 的开发与测试集成环境。本文档面向人类贡献者，说明如何参与本仓库以及上下游子模块的协作。

## 仓库定位

`rmdev-dev` **不是一个产品工程模板**，而是将独立维护的库子模块聚拢到一起，提供集成构建、宿主单元测试与文档聚合的环境。

各库（`emdevif`、`rmdev`、`emdevif_stm32_peripheral` 等）本身可以**独立使用**，各自有独立的仓库、版本历史与 CI。它们的贡献指南见各自仓库内的 `CONTRIBUTING.md`（本文档的子模块贡献入口章节有链接）。

## 编码规范

所有代码贡献必须遵循统一编码规范，详见 [docs/coding-style.md](docs/coding-style.md)。该规范覆盖命名、代码格式、Doxygen 注释、工程约束、模块扩展指引与验证要求。

## 多仓库协作流程

`rmdev-dev` 是集成与验证环境，库逻辑的日常开发应在对应子模块的独立仓库完成：

1. **库逻辑改动**：优先在子模块独立仓库（如 `emdevif.git`、`rmdev_math.git`、`rmdev_driver_BMI088.git`）提交并验证；
2. **同步到集成环境**：子模块改动合并后，及时同步到 `rmdev-dev` 并运行宿主测试，确保集成路径无回归；
3. **集成层改动**：仅当改动涉及 `rmdev-dev` 自身的构建脚本、测试框架、文档聚合或 CI 配置时，才直接在 `rmdev-dev` 提交；
4. **子模块聚合层**：`rmdev` 与 `emdevif_collection/*` 在本仓库中作为子模块引入，如需调整其 `CMakeLists.txt`、`.gitmodules` 或聚合结构，按第 3 条处理。

## 子模块贡献入口

| 子模块 | 贡献指南 |
|---|---|
| emdevif（嵌入式通用接口抽象） | [emdevif_collection/emdevif/CONTRIBUTING.md](emdevif_collection/emdevif/CONTRIBUTING.md) |
| emdevif_stm32_peripheral（STM32 外设扩展） | [emdevif_collection/emdevif_stm32_peripheral/CONTRIBUTING.md](emdevif_collection/emdevif_stm32_peripheral/CONTRIBUTING.md) |
| rmdev（电控算法/驱动聚合库） | [rmdev/CONTRIBUTING.md](rmdev/CONTRIBUTING.md) |

各子模块的 `CONTRIBUTING.md` 同样引用了本仓库的 `docs/coding-style.md` 作为统一编码规范，并链接回本文档作完整流程参考。

## Git 提交规范（对所有子模块都适用）

- 提交信息统一使用中文，清晰描述变更内容和原因。
- 提交信息第一行为一句简洁的总结，基于最近一次提交与当前变更之间的差异概括。
- 若变更较复杂，可在第一行后空一行，以 `- ` 开头的条目补充原因、内容和影响；简单变更可省略。
- 提交信息每行首字母大写，不以句号结尾，不使用 Markdown 语法。
- 最后一行标注 AI 辅助信息，格式为
  `Assisted-by: AGENT_NAME:MODEL_VERSION[, AGENT_NAME2:MODEL_VERSION2, ...] [TOOL1] [TOOL2]...`
  - AGENT_NAME：你使用的 AI 工具、框架或智能体的名称（例如 Claude, Copilot, Codex 等）。
  - MODEL_VERSION：具体调用的模型版本（例如 claude-3-opus, gpt-4 等）。
  - `[TOOL1]` `[TOOL2]`（可选）：搭配使用的专业代码分析工具（例如 coccinelle, sparse, smatch, clang-tidy 等）。
  - 若 MODEL_VERSION 包含空格，需用英文双引号将其包裹，例如 "K2.7 Code"。
  - 若包含多个 AGENT_NAME:MODEL_VERSION，则用逗号分隔。
  - 示例：
    - `Assisted-by: Codex:ChatGPT-4.5`
    - `Assisted-by: OpenCode:deepseek-v4-pro clang-tidy`
    - `Assisted-by: Claude:claude-3-opus coccinelle sparse`
    - `Assisted-by: Kimi Code:"K2.7 Code", OpenCode:GLM-5.1 clang-tidy`
- 多行提交信息使用多个 `-m` 参数分行，不要用 `\n` 内嵌换行。

## 如何贡献

1. Fork 本仓库并创建你的特性分支。
2. 修改代码并遵循 `docs/coding-style.md` 中的编码规范。
3. 确保改动通过宿主单元测试（见 [README.md](README.md) 快速开始章节）。
4. 创建 Pull Request，描述变更内容和原因。
5. 等待 Review 与 CI 通过后合并。

对于子模块库的改动，请先在其独立仓库中贡献，再同步到 `rmdev-dev` 做集成验证。
