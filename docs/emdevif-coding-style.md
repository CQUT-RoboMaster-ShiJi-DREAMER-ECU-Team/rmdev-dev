# emdevif 编码规范

本文档面向所有贡献者，说明编写与阅读 `emdevif` 及下游 `rmdev` 代码时应遵守的编码规则与工程约束。

---

## 1. 代码格式规范（强制）

项目格式以仓库 `.clang-format` 为准，核心约束如下：

- 基础风格：`BasedOnStyle: Google`
- 缩进：4 空格，不使用 Tab
- 行宽：`ColumnLimit: 120`
- 指针/引用：左对齐（`int* p`, `int& r`）
- include：不自动排序（`SortIncludes: Never`）
- case 标签不额外缩进（`IndentCaseLabels: false`）

### 1.1 大括号与换行

- 类、函数定义的大括号单独换行；struct、控制语句、命名空间的大括号不换行。
- 控制语句（`if` / `for` / `while` / `do-while`）必须添加大括号，哪怕只有一行。

### 1.2 空格与对齐

- 二元运算符两侧保留空格。
- 逗号后空格。
- 尾注释与代码之间保留至少 2 个空格。

### 1.3 注释规范

- 注释要解释"为什么"，而非重复"代码在做什么"。
- 公共接口、非直观行为、约束条件必须有注释。
- 不要保留失效的注释或大段被注释掉的旧代码。

---

## 2. 命名规范

项目同时包含模块名、文件名、C API、C++ API，需分层遵循一致约定。

### 2.1 文件与目录命名

- 目录名：`snake_case`（如 `peripheral_handle_map`）
- 头文件名：`snake_case.hpp` / `snake_case.h`
- C++ 模块文件名：`snake_case.cppm`
- 如果存在遗留的不规范命名（如 `ErrorHandler.hpp`），新文件应遵循规范，旧文件可保留但不再新增同目录不规范命名。

### 2.2 CMake 目标命名

- `emdevif` 子模块目标统一前缀：`emdevif_`
- 示例：`emdevif_core`、`emdevif_logger`、`emdevif_system`
- 内部实现目标命名以下划线结尾，如 `emdevif_peripheral_impl_`。
- `rmdev` 子模块目标统一前缀：`rmdev_`（如 `rmdev_math`、`rmdev_driver_BMI088`）

### 2.3 命名空间

- 顶层命名空间统一为：`emdevif`
- 命名风格采用 `snake_case`，避免与类型/函数命名混淆。
- 用户注入实现命名空间固定：`emdevif::user_declares`
- 模块内子命名空间使用语义化小写（如 `logger`, `timeline`）

### 2.4 类型、函数、变量、常量

- 类型名（类 / 结构体 / 枚举）：`PascalCase`
  - 示例：`ErrorCode`, `InitGuard`, `SerialModel`
- 函数名：`camelCase`
  - 示例：`registerFatalHandler`, `getMicroseconds`
- 变量名：`snake_case`
  - 示例：`peripheral_handle_map`, `message_queue`
- 常量：
  - 宏常量：`UPPER_SNAKE_CASE`
  - `constexpr` 常量视作变量，使用 `snake_case`。

### 2.5 宏命名

- 宏名必须全大写 + 下划线，且带模块前缀避免污染：
  - `EMDEVIF_*`
  - `EMDEVIF_TEST_*`
  - `RMDEV_*`（rmdev 外部可见宏）
- 宏参数必须加括号，避免副作用与优先级问题。

---

## 3. C 与 C++ 边界规范

- `.h`：优先保证 C/C++ 兼容。
- `.hpp` / `.cppm`：C++ 专用。
- 对外暴露 C 接口时：
  - 在 C++ 下使用 `EMDEVIF_EXTERN_C` / `EMDEVIF_EXTERN_C_BEGIN` / `EMDEVIF_EXTERN_C_END` 宏封装。
  - 头文件防重包含宏与 `#pragma once` 保持项目既有风格一致。

---

## 4. C++20 模块规范

- 模块名与头文件路径语义保持一致（如 `emdevif.core.error_handler` 对应 `emdevif/core/error_handler.hpp`）。
- 模块接口单元应只导出必要符号，避免泄露实现细节。
- 不在模块接口中引入不必要的大型依赖。
- 避免同一接口被"模块导入 + 头文件包含"混用导致 ODR 或编译问题。

---

## 5. 工程约束

以下规则补充前文规范，涵盖代码质量、依赖管理和边界约定的整体要求。

### 5.1 工程原则

- 可读性优先于技巧性，避免不必要的复杂实现。
- 公共接口稳定优先，最小化破坏性变更。
- 平台解耦，避免把板级细节引入核心层。

### 5.2 静态检查

- 不引入新的高置信 `.clang-tidy` 告警。
- 重点关注：`bugprone-*`、`cppcoreguidelines-*`、`modernize-*`、`performance-*`、`readability-*`。

### 5.3 错误处理

- 返回显式错误码，不静默失败。
- 断言用于开发期不变量，不替代运行时错误处理。

### 5.4 依赖与边界

- include 最小化，避免公共头重依赖。
- 禁止循环依赖，优先前置声明。
- 用户实现入口经 `emdevif_user_declares` 注入。
- 禁止修改第三方代码：`core/depends/**`。

### 5.5 测试与文档同步

- 公共接口变化应同步更新测试、示例和文档。
- 测试命名保持与忽略列表机制兼容。

---

## 6. 提交前自查

修改或新增代码后，建议逐项过一遍：

- 格式与命名是否符合本文档约定
- 控制语句是否都加了括号，注释是否解释"为什么"
- 宏命名是否使用正确模块前缀，宏参数是否加了括号
- 是否引入了关键静态检查告警
- 模块边界与接口稳定性是否完好
- 相关测试与文档是否已同步
- 涉及 C++20 模块时，是否已验证 `EMDEVIF_USE_CPP_MODULES=ON/OFF` 两条构建路径

---

## 7. 总体开发原则

### 7.1 emdevif 开发原则

- `core` 为必选基础层，其他模块可按需裁剪。
- 保持"模块导入"和"头文件导入"两种方式接口语义一致。
- 需用户实现的底层能力统一放到 `emdevif_user_declares`，避免循环依赖。
- 不在 `emdevif` 主模块内引入强板级耦合实现。
- API 稳定性优先，谨慎变更错误处理、断言和终止流程。
- 保持 C / C++ 边界清晰（`.h` 与 `.hpp`）。

### 7.2 rmdev 开发原则

- 保持可裁剪特性：基础模块默认启用，INS 与驱动按需启用。
- 新能力优先模块化拆分，避免在聚合层堆积板级细节。
- 新驱动接入遵循 `RMDEV_ENABLED_DRIVER_LIST` 机制。
- `rmdev` 依赖 `emdevif`，必须先 `add_subdirectory(emdevif)` 再引入 `rmdev`。

## 8. 扩展指引

### 8.1 emdevif 新增外设类型

emdevif 本身不直接操作外设，而是通过 `peripheral/` 子系统提供统一的 Model → Impl 解耦框架。新增一种外设类型（如 I2C、ADC）时，在 `peripheral/` 下按三层架构添加，详见 peripheral 模块的 AGENTS.md。

### 8.2 emdevif 新增功能模块

如需添加全新的库级模块（如数学工具、数据压缩等）：

1. 在仓库根目录创建模块目录（如 `compression/`），内建 `inc/` + `src/` + `CMakeLists.txt`
2. CMake 目标命名为 `emdevif_<module>`
3. 在根 `CMakeLists.txt` 的 `emdevifAllModules` 列表中加入新模块
4. 在本文件中新增对应的子模块约束小节

### 8.3 rmdev 新增功能模块

1. 在独立仓库中开发模块，遵循 `rmdev_<name>` 命名
2. 在 `rmdev/modules/` 下以 git submodule 引入
3. 在 `rmdev/CMakeLists.txt` 中添加 `add_subdirectory` 和 `target_link_libraries`
4. 在本文件中新增对应的子模块约束小节
5. 如需按条件启用，选择合适的机制：布尔开关（类似 INS）或列表（类似驱动）

### 8.4 rmdev 新增驱动

1. 在独立仓库中开发驱动，遵循 `rmdev_driver_<Name>` 命名
2. 在 `rmdev/drivers/` 下以 git submodule 引入
3. 用户通过 `RMDEV_ENABLED_DRIVER_LIST` 启用
4. 驱动通过 `emdevif_peripheral` 访问硬件，不直接耦合板级

---

## 9. 验证要求

### 9.1 emdevif 验证

- 至少验证 `EMDEVIF_USE_CPP_MODULES=ON/OFF` 两条构建路径。
- 变更涉及 `system` / `logger` / `timeline` 时，验证用户声明接口路径可用。
- 变更涉及 `support/` 时，至少验证 STM32CubeMX 和 ESP-IDF 各一次构建。
- 修改公共头文件时，运行相关测试套件（test_suits 下对应目录）。

### 9.2 rmdev 验证

- 检查 `RMDEV_ENABLE_INS_MODULE` 与驱动列表的条件构建路径。
- 关键模块改动后，至少验证对应测试或最小集成编译。
- 与 emdevif 接口交互的改动，需验证 `EMDEVIF_USE_CPP_MODULES=ON/OFF` 两条路径。
- 变更涉及子模块子仓库时，还需在该子仓库独立验证。
