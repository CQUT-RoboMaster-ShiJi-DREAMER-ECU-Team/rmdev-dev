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
├── tests/                        # 模拟单元测试
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
- [emdevif/AGENTS.md](emdevif_collection/emdevif/AGENTS.md) — core/logger/peripheral/system/timeline 各模块约束，含 C++20 Modules 双路径验证
- [emdevif_stm32_peripheral/AGENTS.md](emdevif_collection/emdevif_stm32_peripheral/AGENTS.md) — HAL/LL 适配与驱动选择路径

## 编码规范

统一编码规范位于本仓库 `docs/coding-style.md`（覆盖 emdevif 与 rmdev 全部子模块）。**身处本仓库时直接阅读该本地文件**；子模块自身文档中的 GitHub 外链指向的也是它，无需跳转。

## 构建系统

### 顶层 CMake 流程

根 `CMakeLists.txt` 根据 `TEST_PLATFORM` 选择入口：

- `TEST_PLATFORM=mock`：模拟单元测试模式，进入 `tests/`，不依赖平台/板卡
- 其他 `TEST_PLATFORM`：暂时未实现，后续添加相关功能

第三方依赖通过 CPM 管理（`cmake/add_packages.cmake`），目前引入 `mpaland/printf` 和 `mpusz/mp-units`。测试依赖 `google/googletest`。

### 关键 CMake 变量

| 变量                        | 类型     | 默认                       | 说明                                              |
|---------------------------|--------|--------------------------|-------------------------------------------------|
| `TEST_PLATFORM`           | String | `"mock"`                 | 见前文                                             |
| `EMDEVIF_USE_CPP_MODULES` | Bool   | `ON`                     | `emdevif` 子模块自身默认 `ON`；本仓库 `CMakePresets.json` 显式覆盖以覆盖 Modules ON/OFF 两条验证路径 |
| `TEST_ENABLE_EXCEPTIONS`  | Bool   | `OFF`                    | C++ 异常开关（模拟测试路径），`CMakeLists.txt` 中默认关闭，Preset 可显式开启；关闭时测试中异常相关用例通过 `GTEST_SKIP` 跳过 |

### 模拟单元测试

在 Windows/Linux/macOS 上使用 GoogleTest 进行单元测试。

当前已覆盖：

- `emdevif/core` 的核心能力
- `rmdev/debug_assistance`
- `rmdev/message_manager`

`emdevif` 的 `logger`、`timeline`、`system`、`peripheral` 以及 `rmdev` 的 `math`、`control_algorithm`、`kinematic_solution`、`device_model`、`ins` 和各驱动模块的测试将在后续补充。

```bash
# 列出所有可用预设
cmake --list-presets

# 配置（4 选 1）
cmake --preset MockTestModulesExceptions
cmake --preset MockTestModulesNoexceptions
cmake --preset MockTestHeadersExceptions
cmake --preset MockTestHeadersNoexceptions

# Debug 构建并运行测试（以 `MockTestModulesExceptions` 配置预设为例）
cmake --build build/mock/MockTestModulesExceptions --config Debug
ctest --test-dir build/mock/MockTestModulesExceptions -C Debug

# Release 构建并运行测试（以 `MockTestModulesExceptions` 配置预设为例）
cmake --build build/mock/MockTestModulesExceptions --config Release
ctest --test-dir build/mock/MockTestModulesExceptions -C Release
```

### 模拟测试 Preset 表

| Preset 名 | EMDEVIF_USE_CPP_MODULES | TEST_ENABLE_EXCEPTIONS |
|---|---|---|
| `MockTestModulesExceptions` | ON | ON |
| `MockTestModulesNoexceptions` | ON | OFF |
| `MockTestHeadersExceptions` | OFF | ON |
| `MockTestHeadersNoexceptions` | OFF | OFF |

每个 configure preset 通过 Ninja Multi-Config 同时支持 Debug 和 Release 两种构建配置。

### 辅助脚本

仓库提供了 `scripts/` 下的 PowerShell 辅助脚本：

- **`Run-MockTests.ps1`**：一键配置、构建并运行预设的单元测试，支持 `-TestSuit` 选择预设组合，`-GeneratorConfig` 指定构建配置，`-Quiet`/`-OutputOnFailure` 控制输出
- **`Format-CppCode.ps1`**：递归格式化 `emdevif_collection/`、`rmdev/`、`tests/` 下的 `.c/.cpp/.h/.hpp/.cppm` 文件，依赖 `clang-format`

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

## 贡献流程与提交规范

**所有提交（包括智能体提交）必须严格遵守 [`CONTRIBUTING.md`](CONTRIBUTING.md) 中的 Git 提交规范。**

在提交代码前，务必阅读 `CONTRIBUTING.md` 的以下章节：
- **Git 提交规范**（含提交信息格式、`Assisted-by` 行等）
- **多仓库协作流程**（何时在子模块提交、何时在本仓库提交）

不规范的提交信息将被要求修改。智能体提交时尤其要注意正确添加 `Assisted-by` 行。
