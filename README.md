# rmdev-dev

`emdevif` 与 `rmdev` 的**开发 / 测试集成环境**。

> 这个仓库不是产品工程模板，而是把各个独立的库聚拢到一起，做集成构建、模拟单元测试与文档聚合。库本身可以脱离本仓库独立使用。

## 这个仓库是什么

`emdevif`（嵌入式开发接口抽象）和 `rmdev`（RoboMaster 电控开发库）都是**独立维护**的库，各有自己的 Git 仓库、版本历史与 CI。它们各自都能被单独克隆、集成到任何工程中。

`rmdev-dev` 把它们以 git subtree 的形式聚拢在一起，提供：

- 统一的模拟（Windows / Linux / macOS）单元测试环境
- 跨库的集成构建验证（含 C++20 模块 ON/OFF 双路径）
- 统一的编码规范与开发流程文档
- 代码格式化、测试运行等辅助脚本

> 子目录 `rmdev/`、`emdevif_collection/emdevif/`、`emdevif_collection/emdevif_stm32_peripheral/` 已通过 git subtree 直接集成在仓库中：克隆即用，无需额外的子模块初始化步骤。所有开发统一在 `rmdev-dev` 进行，子仓库作为发布镜像由维护者通过 git subtree 推送回去；子仓库已关闭 Pull Request，仅保留 Issue。

## 仓库结构

```
rmdev-dev/
├── rmdev/                        # 电控算法/驱动聚合库（独立仓库，git subtree 集成）
├── emdevif_collection/
│   ├── emdevif/                  # 嵌入式通用接口抽象（独立仓库，git subtree 集成）
│   └── emdevif_stm32_peripheral/ # emdevif 的 STM32 外设扩展（独立仓库，git subtree 集成）
├── tests/                        # 模拟单元测试（GoogleTest）
├── docs/
│   └── coding-style.md           # 统一编码规范（emdevif + rmdev 通用）
├── cmake/                        # CMake 辅助脚本
└── scripts/                      # PowerShell 辅助脚本
```

每个子仓库目录下都有自己的 `README.md`（面向人类的使用指南）和 `AGENTS.md`（面向协作智能体的约束说明）。

## 快速开始：运行模拟单元测试

本仓库目前实现的验证路径是模拟单元测试（`TEST_PLATFORM=mock`），不依赖任何板卡或工具链。

### 方式一：使用预设（推荐）

```bash
# 列出所有可用预设
cmake --list-presets

# 配置（4 选 1）
cmake --preset MockTestModulesExceptions
cmake --preset MockTestModulesNoexceptions
cmake --preset MockTestHeadersExceptions
cmake --preset MockTestHeadersNoexceptions

# 构建并测试（以 MockTestModulesExceptions 为例，支持 Debug / Release）
cmake --build build/mock/MockTestModulesExceptions --config Debug
ctest --test-dir build/mock/MockTestModulesExceptions -C Debug
```

四个预设覆盖了「C++20 模块 开/关」×「异常 开/关」两条验证路径的组合：

| Preset | EMDEVIF_USE_CPP_MODULES | TEST_ENABLE_EXCEPTIONS |
|---|---|---|
| `MockTestModulesExceptions`    | ON  | ON  |
| `MockTestModulesNoexceptions`  | ON  | OFF |
| `MockTestHeadersExceptions`    | OFF | ON  |
| `MockTestHeadersNoexceptions`  | OFF | OFF |

每个预设基于 Ninja Multi-Config，同时支持 Debug 和 Release。

### 方式二：使用辅助脚本

```powershell
# 查看帮助
Get-Help ./scripts/Run-MockTests.ps1

# 一键编译并运行所有预设的单元测试（Release）
./scripts/Run-MockTests.ps1

# 仅运行指定预设（可组合短名或长名）
./scripts/Run-MockTests.ps1 -TestSuit "ME", "MNE" -GeneratorConfig Debug

# 格式化仓库内所有 C++ 代码（需安装 clang-format）
./scripts/Format-CppCode.ps1
```

## 文档导航

| 你想了解 | 去哪里看 |
|---|---|
| 编码规范（命名 / 格式 / Doxygen / 工程约束） | [docs/coding-style.md](docs/coding-style.md) |
| 贡献指南（开发与发布流程 / 提交规范 / 如何贡献） | [CONTRIBUTING.md](CONTRIBUTING.md) |
| 智能体协作约束（修改规则 / 验证要求） | [AGENTS.md](AGENTS.md) |
| emdevif 怎么用 | [emdevif_collection/emdevif/README.md](emdevif_collection/emdevif/README.md) |
| rmdev 怎么用 | [rmdev/README.md](rmdev/README.md) |
| STM32 外设封装 | [emdevif_collection/emdevif_stm32_peripheral/README.md](emdevif_collection/emdevif_stm32_peripheral/README.md) |

> 提示：子仓库文档里出现的「编码规范」GitHub 外链，指向的就是本仓库的 `docs/coding-style.md`。既然你已经在 `rmdev-dev` 里，直接看本地这份即可。

## 克隆与初始化

```bash
git clone https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev.git
```

子目录 `rmdev/`、`emdevif_collection/emdevif/`、`emdevif_collection/emdevif_stm32_peripheral/` 已通过 git subtree 直接合入仓库，克隆后即可直接构建与测试，无需任何额外的子模块初始化步骤。

若只需使用其中某个库（如 `emdevif`），可直接克隆对应子仓库：

```bash
git clone https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif.git
```
