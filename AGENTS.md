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
根 `CMakeLists.txt` 根据 `PLATFORM_NAME` 选择入口：

- `stm32`：进入 `RunningPlatforms/STM32/${BOARD_NAME}`，使用 GCC ARM 工具链
- `esp32`：委托 ESP-IDF 构建系统，需已安装 ESP-IDF 环境

第三方依赖通过 CPM 管理（`cmake/add_packages.cmake`），目前引入 `mpaland/printf` 和 `mpusz/mp-units`。

### 关键 CMake 变量

| 变量 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `PLATFORM_NAME` | String | `""` | `stm32` 或 `esp32` |
| `BOARD_NAME` | String | `""` | 开发板目录名（如 `DM-MC-Board02`） |
| `RMDEV_ENABLE_TESTS` | Bool | `ON` | 是否构建并运行 rmdev 测试 |
| `EMDEVIF_USE_CPP_MODULES` | Bool | 由子模块决定 | C++20 Modules 路径开关，变更后需验证 ON/OFF 两条路径 |

### 构建示例

STM32（通过 `CMakePresets.json`，所有预设均已内置工具链、平台与板卡参数）：

```bash
# 配置
cmake --preset RoboMasterDevelopmentBoardTypeC
# 构建（默认 Debug）
cmake --build --preset RoboMasterDevelopmentBoardTypeC-Debug
# 或构建 Release
cmake --build --preset RoboMasterDevelopmentBoardTypeC-Release
# 列出所有可用预设
cmake --list-presets
```

可用的 configure 预设：

| 预设名 | 对应板卡 |
|---|---|
| `RoboMasterDevelopmentBoardTypeC` | RoboMaster 开发板 C 型 (STM32F407IGHx) |
| `ShiJiDreamerDevBoardF4` | 士继 DREAMER 开发板 (STM32F407VGT6) |
| `DM-MC-Board02` | 达妙 DM-MC-Board02 (STM32H723VGT6) |
| `F103C8T6` | STM32F103C8T6 最小系统板 |

每个 configure 预设均有对应的 `-Debug` / `-Release` build 预设。

ESP32（需已安装 ESP-IDF，不使用 CMake preset）：

```bash
idf.py -DPLATFORM_NAME=esp32 -DBOARD_NAME=ESP32-DevKitC build
```

### cmake-build-* 目录
根目录下的 `cmake-build-debug-*` / `cmake-build-release-*` 是 CLion 生成的 out-of-tree 构建目录，由 `.gitignore` 排除，不要手动编辑其中的文件。

## 测试入口契约

`TestImplement/` 暴露两个 C 链接函数，板级启动代码必须调用：

```c
// 初始化（STM32 在 HAL_Init 前调用，ESP32 可调用也可不调用）
void testInit(void* argument, ...);

// 进入测试循环（创建 RTOS 任务并启动调度器 / 直接执行）
EMDEVIF_NO_RETURN void testEntry(void);
```

板级工程负责提供 `testInit` / `testEntry` 的调用时机，测试实现本身根据 `PLATFORM_NAME` 编译 `stm32_test_impl.cpp` 或 `esp32_test_impl.cpp`。

## 新增板卡 / 平台

### 新增 STM32 板卡
在 `RunningPlatforms/STM32/` 下创建以板卡名命名的目录，提供：

- `cmake/gcc-arm-none-eabi.cmake` — 工具链文件
- `Core/` — CubeMX 生成的 HAL 初始化代码（含 `main` 中调用 `testInit` / `testEntry`）
- `Drivers/` — HAL 库
- `CMakeLists.txt` — 板级 CMake

可参考现有板卡（如 `RoboMasterDevelopmentBoardTypeC`）的结构。

### 新增 ESP32 板卡
在 `RunningPlatforms/ESP32/` 下创建目录，提供：

- `main/` — ESP-IDF 组件（含 CMakeLists.txt 和测试入口）
- `sdkconfig.defaults` — 默认 Kconfig 覆盖

然后在 README 中补充板卡接线说明。

### 新增平台（如 RP2040）
需要在根 `CMakeLists.txt` 的 `if/elseif/else` 分支中新增平台判断逻辑，并在 `RunningPlatforms/` 下创建对应平台目录。

## 关键配置文件

| 文件 | 作用 |
|---|---|
| `CMakePresets.json` | CMake 预设（统一管理各板卡的 generator、工具链与构建类型） |
| `.clang-format` / `.clang-tidy` | C/C++ 代码风格与静态检查规则 |
| `.clangd` | LSP 配置（后台索引与补全） |
| `ftdi.cfg` / `stlink.cfg` / `daplink.cfg` | OpenOCD 调试器配置文件 |
| `FreeMaster_DM-MC-02_DemoWatcher.pmpx` | FreeMaster 运行时变量监视配置 |
| `cmake/utils.cmake` | `addCopyFinallyBinaryFileTarget`：构建后将二进制自动拷贝到 `bin/` |

## C++20 Modules 注意

代码同时支持 `#include` 和 `import` 两种导入方式，通过 `EMDEVIF_USE_CPP_MODULES` 切换。emdevif 子模块的 AGENTS.md 要求验证 ON/OFF 两条构建路径。在本仓库做集成验证时，至少覆盖一条 Modules 开启路径和一条关闭路径。

## 项目技能

本项目的技能文件存放在 `.agents/skills/` 目录下。

### `cpp-lib-headers-modules-both-supports`

指导如何设计 C++ 库使其同时支持头文件（`#include`）和 C++20 模块（`import`）两种使用方式，涵盖纯头文件库、组件依赖、带源文件的库等场景。
具体模式参见技能目录下的 SKILL.md。

## 修改约束
- 优先在对应子模块中修复库逻辑；本仓库只做集成与验证层调整。
- 不修改 `emdevif_collection/emdevif/core/depends/` 第三方代码。
- 与平台无关的测试逻辑放在 `TestImplement/`，平台相关逻辑放在 `RunningPlatforms/`。
- `.agents/` 目录下的智能体规则与技能文档优先于其他历史路径；项目技能统一存放在 `.agents/skills/` 下。
- 遇到已有的不符合 Doxygen 注释规范的代码时，不要强制修改格式，应先询问用户是否需要修改。

## Git 提交规范（对所有子模块都适用）

- 提交信息统一使用中文，清晰描述变更内容和原因。
- 仅当用户明确要求提交时才执行 `git commit`，不要因检测到暂存区有修改而自动提交。
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

## 验证建议
- 使用 `cmake --preset <板卡名>` 配置，`cmake --build --preset <板卡名>-Debug` 构建。
- 变更板级逻辑后，至少在对应板卡配置完成一次构建验证。
- 变更涉及 `TestImplement/` 时，至少验证一个 STM32 板卡和一个 ESP32 板卡的构建。
