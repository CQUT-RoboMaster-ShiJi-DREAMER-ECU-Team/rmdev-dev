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
├── RunningPlatforms/             # 平台与板级工程（按平台/开发板划分）
│   ├── STM32/                    #   STM32 板卡工程（CubeMX + CMake）
│   └── ESP32/                    #   ESP32 板卡工程（ESP-IDF）
├── TestImplement/                # 跨平台测试实现与测试入口适配
├── cmake/                        # CMake 辅助脚本（utils、add_packages、get_cpm）
└── bin/                          # 构建产物自动拷贝目标
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
| `EMDEVIF_USE_CPP_MODULES` | Bool   | 由 `CMakePresets.json` 决定 | C++20 Modules 路径开关，变更后需验证 ON/OFF 两条路径           |
| `TEST_ENABLE_EXCEPTIONS`  | Bool   | `ON`                     | C++ 异常开关（宿主测试路径），关闭时测试中异常相关用例通过 `GTEST_SKIP` 跳过 |

### 宿主单元测试

在 Windows/Linux/macOS 上使用 GoogleTest 进行单元测试，覆盖 emdevif 和 rmdev 的核心模块。

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

## Git 提交规范（对所有子模块都适用）

- 提交信息统一使用中文，清晰描述变更内容和原因。
- 提交信息第一行为一句简洁的总结，基于最近一次提交与当前变更之间的差异概括。
- 若变更较复杂，可在第一行后空一行，以 `- ` 开头的条目补充原因、内容和影响；简单变更可省略。
- 提交信息每行首字母大写，不以句号结尾，不使用 Markdown 语法。
- 最后一行标注 AI 辅助信息，格式为 `Assisted-by: AGENT_NAME:MODEL_VERSION [TOOL1] [TOOL2]...`。
  - AGENT_NAME：你使用的 AI 工具、框架或智能体的名称（例如 Claude, Copilot, Codex 等）。
  - MODEL_VERSION：具体调用的模型版本（例如 claude-3-opus, gpt-4 等）。
  - `[TOOL1]` `[TOOL2]`（可选）：搭配使用的专业代码分析工具（例如 coccinelle, sparse, smatch, clang-tidy 等）。
  - 示例：
    - `Assisted-by: Codex:ChatGPT-4.5`
    - `Assisted-by: OpenCode:deepseek-v4-pro clang-tidy`
    - `Assisted-by: Claude:claude-3-opus coccinelle sparse`
- 多行提交信息使用多个 `-m` 参数分行，不要用 `\n` 内嵌换行。
