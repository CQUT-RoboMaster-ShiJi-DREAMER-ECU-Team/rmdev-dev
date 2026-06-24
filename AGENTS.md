# AGENTS.md

## 仓库定位

该仓库是 `rmdev` 与 `emdevif` 的开发/测试集成环境，不是最终产品工程模板。

## 仓库结构

```
rmdev-dev/
├── rmdev/                        # 子模块：电控算法/驱动聚合库（有独立 AGENTS.md）
├── emdevif_collection/
│   ├── emdevif/                  # 子模块：嵌入式通用接口抽象（有独立 AGENTS.md）
│   └── emdevif_stm32_peripheral/ # 子模块：emdevif 的 STM32 外设扩展（有独立 AGENTS.md）
├── tests/                        # 宿主单元测试
│   ├── CMakeLists.txt
│   └── mock/
│       ├── host_test_pch.hpp     # 预编译头（GoogleTest）
│       ├── CMakeLists.txt        # GoogleTest CPM 集成与测试注册
│       ├── emdevif/              # emdevif 各模块测试（镜像源目录结构）
│       └── rmdev/                # rmdev 各模块测试（镜像源目录结构）
└── cmake/                        # CMake 辅助脚本（utils、add_packages、get_cpm）
```

### 子模块层级

三个子模块各有独立 AGENTS.md，修改库逻辑时应遵循对应模块的约束：

- [rmdev/AGENTS.md](rmdev/AGENTS.md) — 算法/模型/驱动模块的裁剪、接口与验证要求
- [emdevif/AGENTS.md](emdevif_collection/emdevif/AGENTS.md) — core/logger/peripheral/system/timeline/util 各模块约束，含 C++20 Modules 双路径验证
- [emdevif_stm32_peripheral/AGENTS.md](emdevif_collection/emdevif_stm32_peripheral/AGENTS.md) — HAL/LL 适配与驱动选择路径

## 构建系统

### 顶层 CMake 流程

根 `CMakeLists.txt` 根据 `TEST_PLATFORM` 选择入口：

- `TEST_PLATFORM=mock`：宿主单元测试模式，进入 `tests/`，不依赖平台/板卡
- 其他 `TEST_PLATFORM`：暂时未实现，后续添加相关功能

第三方依赖通过 CPM 管理（`cmake/add_packages.cmake`），目前引入 `mpaland/printf` 和 `mpusz/mp-units`。测试依赖 `google/googletest`。

### 关键 CMake 变量

| 变量                        | 类型     | 默认                       | 说明                                              |
|---------------------------|--------|--------------------------|-------------------------------------------------|
| `TEST_PLATFORM`           | String | `"mock"`                 | 见前文                                             |
| `EMDEVIF_USE_CPP_MODULES` | Bool   | `ON`                     | `emdevif` 子模块自身默认 `ON`；本仓库 `CMakePresets.json` 显式覆盖以覆盖 Modules ON/OFF 两条验证路径 |
| `TEST_ENABLE_EXCEPTIONS`  | Bool   | `OFF`                    | C++ 异常开关（宿主测试路径），`CMakeLists.txt` 中默认关闭，Preset 可显式开启；关闭时测试中异常相关用例通过 `GTEST_SKIP` 跳过 |

### 宿主单元测试

在 Windows/Linux/macOS 上使用 GoogleTest 进行单元测试。

当前已覆盖：

- `emdevif/core` 的核心能力
- `rmdev/debug_assistance`
- `rmdev/message_manager`

`emdevif` 的 `logger`、`timeline`、`system`、`peripheral` 以及 `rmdev` 的 `rmdev_math`、`rmdev_control_algorithm`、`rmdev_kinematic_solution`、`rmdev_device_model`、`rmdev_ins` 和各驱动模块的测试将在后续补充。

```bash
# 列出所有可用预设
cmake --list-presets

# 配置（4 选 1）
cmake --preset HostTestModulesExceptions
cmake --preset HostTestModulesNoexceptions
cmake --preset HostTestHeadersExceptions
cmake --preset HostTestHeadersNoexceptions

# Debug 构建并运行测试
cmake --build --preset HostTestModulesExceptions --config Debug
ctest --preset HostTestModulesExceptions -C Debug

# Release 构建并运行测试
cmake --build --preset HostTestModulesExceptions --config Release
ctest --preset HostTestModulesExceptions -C Release
```

### 宿主测试 Preset 表

| Preset 名 | EMDEVIF_USE_CPP_MODULES | TEST_ENABLE_EXCEPTIONS |
|---|---|---|
| `HostTestModulesExceptions` | ON | ON |
| `HostTestModulesNoexceptions` | ON | OFF |
| `HostTestHeadersExceptions` | OFF | ON |
| `HostTestHeadersNoexceptions` | OFF | OFF |

每个 configure preset 通过 Ninja Multi-Config 同时支持 Debug 和 Release 两种构建配置。

## C++20 Modules 注意

代码同时支持 `#include` 和 `import` 两种导入方式，通过 `EMDEVIF_USE_CPP_MODULES` 切换。emdevif 子模块的 AGENTS.md 要求验证 ON/OFF 两条构建路径。在本仓库做集成验证时，至少覆盖一条 Modules 开启路径和一条关闭路径。

## 项目技能

本项目的技能文件存放在 `.agents/skills/` 目录下。

### `cpp-lib-headers-modules-both-supports`

指导如何设计 C++ 库使其同时支持头文件（`#include`）和 C++20 模块（`import`）两种使用方式，涵盖纯头文件库、组件依赖、带源文件的库等场景。
具体模式参见技能目录下的 SKILL.md。

## 修改约束

- 优先在对应子模块中修复库逻辑；本仓库只做集成与验证层调整。
- 测试逻辑放在 `tests/mock/`。
- `.agents/` 目录下的智能体规则与技能文档优先于其他历史路径；项目技能统一存放在 `.agents/skills/` 下。
- 遇到已有的不符合 Doxygen 注释规范的代码时，不要强制修改格式，应先询问用户是否需要修改。

## 多仓库协作流程

`rmdev-dev` 是集成与验证环境，库逻辑的日常开发应在对应子模块的独立仓库完成：

1. **库逻辑改动**：优先在子模块独立仓库（如 `emdevif.git`、`rmdev_math.git`、`rmdev_driver_BMI088.git`）提交并验证；
2. **同步到集成环境**：子模块改动合并后，及时同步到 `rmdev-dev` 并运行宿主测试，确保集成路径无回归；
3. **集成层改动**：仅当改动涉及 `rmdev-dev` 自身的构建脚本、测试框架、文档聚合或 CI 配置时，才直接在 `rmdev-dev` 提交；
4. **子模块聚合层**：`rmdev` 与 `emdevif_collection/*` 在本仓库中作为子模块引入，如需调整其 `CMakeLists.txt`、`.gitmodules` 或聚合结构，按第 3 条处理。

## Git 提交规范（对所有子模块都适用）

- 提交信息统一使用中文，清晰描述变更内容和原因。
- 提交信息第一行为一句简洁的总结，基于最近一次提交与当前变更之间的差异概括。
- 若变更较复杂，可在第一行后空一行，以 `- ` 开头的条目补充原因、内容和影响；简单变更可省略。
- 提交信息每行首字母大写，不以句号结尾，不使用 Markdown 语法。
- 最后一行标注 AI 辅助信息，格式为 `Assisted-by: AGENT_NAME:MODEL_VERSION[, AGENT_NAME2:MODEL_VERSION2, ...] [TOOL1] [TOOL2]...`。
  - AGENT_NAME：你使用的 AI 工具、框架或智能体的名称（例如 Claude, Copilot, Codex 等）。
  - MODEL_VERSION：具体调用的模型版本（例如 claude-3-opus, gpt-4 等）。
  - `[TOOL1]` `[TOOL2]`（可选）：搭配使用的专业代码分析工具（例如 coccinelle, sparse, smatch, clang-tidy 等）。
  - 若 `MODEL_VERSION` 包含空格，需用英文双引号将其包裹，例如 `"K2.7 Code"`。
  - 若包含多个 `AGENT_NAME:MODEL_VERSION`，则用逗号分隔。
  - 示例：
    - `Assisted-by: Codex:ChatGPT-4.5`
    - `Assisted-by: OpenCode:deepseek-v4-pro clang-tidy`
    - `Assisted-by: Claude:claude-3-opus coccinelle sparse`
    - `Assisted-by: Kimi Code:"K2.7 Code", OpenCode:GLM-5.1 clang-tidy`
- 多行提交信息使用多个 `-m` 参数分行，不要用 `\n` 内嵌换行。
