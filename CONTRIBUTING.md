# 贡献指南

`rmdev-dev` 是 `emdevif` 与 `rmdev` 的开发与测试集成环境。本文档面向所有贡献者（人类与协作智能体），说明如何参与本仓库以及上下游子仓库的协作。

## 仓库定位

`rmdev-dev` **不是一个产品工程模板**，而是将独立维护的库聚拢到一起，提供集成构建、模拟单元测试与文档聚合的环境。

各库（`emdevif`、`rmdev`、`emdevif_stm32_peripheral` 等）本身可以**独立使用**，各自有独立的仓库、版本历史与 CI。用户按需克隆对应子仓库即可使用，不必把 `rmdev-dev` 集成的所有仓库都下载下来。

## 集成模型

`rmdev-dev` 通过 **git subtree** 集成各子仓库：

| 子目录 | 子仓库 |
|---|---|
| `rmdev/` | [rmdev.git](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev.git) |
| `emdevif_collection/emdevif/` | [emdevif.git](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif.git) |
| `emdevif_collection/emdevif_stm32_peripheral/` | [emdevif_stm32_peripheral.git](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif_stm32_peripheral.git) |

子仓库内容直接合入本仓库，**克隆 `rmdev-dev` 即可获取全部代码**，无需任何子模块初始化步骤。所有开发统一在本仓库进行；子仓库已关闭 Pull Request（仍保留 Issue），仅作为发布镜像由维护者通过 `git subtree push` 推送回去。

## 编码规范

所有代码贡献必须遵循统一编码规范，详见 [docs/coding-style.md](docs/coding-style.md)。该规范覆盖命名、代码格式、Doxygen 注释、工程约束、模块扩展指引与验证要求。

## 开发与发布流程

`rmdev-dev` 是 `emdevif` / `rmdev` / `emdevif_stm32_peripheral` 等子仓库的**统一开发入口**：所有代码贡献（库逻辑、构建脚本、模拟测试、文档等）都在本仓库完成并通过 Pull Request 合并。各子仓库只接收 Issue 反馈，不再接收 Pull Request。

将测试与库版本耦合在一起是这一模型的核心价值：库改动与其对应的模拟测试放在同一个 PR 里，合并即可保证版本对应，不再像 submodule 模式那样需要先在子仓库提交、再回到总测试仓库手动对齐版本。

### 贡献者流程

1. Fork `rmdev-dev` 并在对应子目录下完成改动（如 `emdevif_collection/emdevif/`、`rmdev/`、`emdevif_collection/emdevif_stm32_peripheral/`），或按需修改 `tests/`、`cmake/`、`docs/` 等集成层文件；
2. 修改代码并遵循 `docs/coding-style.md` 中的编码规范；
3. 在本仓库运行模拟单元测试（见 [README.md](README.md) 的「快速开始」章节）确保无回归；
4. 创建 Pull Request，描述变更内容与原因；
5. 通过 Review 与 CI 后合并。

合并即完成贡献，**无需**在子仓库再做任何提交或同步——子仓库的同步由维护者统一处理。

### 维护者发布流程

PR 合并到 `rmdev-dev` `main` 后，由维护者通过 git subtree 把子目录的更新推送回对应子仓库：

```bash
# rmdev
git subtree push --prefix=rmdev https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev main

# emdevif
git subtree push --prefix=emdevif_collection/emdevif https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif main

# emdevif_stm32_peripheral
git subtree push --prefix=emdevif_collection/emdevif_stm32_peripheral https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif_stm32_peripheral main
```

需要从子仓库拉取上游修复时，使用对应的 `git subtree pull --prefix=... <url> main --squash`，并配套运行本仓库的模拟单元测试确认无回归。

### 子仓库 Issue

各子仓库仍保留 Issue，用户与贡献者可在对应仓库的 Issue 区反馈问题、提出功能请求；但代码贡献请统一在 `rmdev-dev` 通过 Pull Request 完成。

## 子仓库参考文档

改动库逻辑前请先阅读对应子仓库目录下的约束说明：

| 子仓库 | 参考文档 |
|---|---|
| emdevif（嵌入式通用接口抽象） | [emdevif_collection/emdevif/AGENTS.md](emdevif_collection/emdevif/AGENTS.md) |
| emdevif_stm32_peripheral（STM32 外设扩展） | [emdevif_collection/emdevif_stm32_peripheral/AGENTS.md](emdevif_collection/emdevif_stm32_peripheral/AGENTS.md) |
| rmdev（电控算法/驱动聚合库） | [rmdev/AGENTS.md](rmdev/AGENTS.md) |

各子仓库的 `CONTRIBUTING.md` 同样重申了「统一在 `rmdev-dev` 开发」，并链接回本文档作完整流程参考。

## Git 提交规范（对所有子仓库都适用）

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
