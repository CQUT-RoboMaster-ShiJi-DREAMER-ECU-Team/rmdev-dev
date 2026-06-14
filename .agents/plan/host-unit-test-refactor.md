# 测试重构方案：宿主单元测试

## 概要

将单元测试从单片机（STM32/ESP32）迁移到宿主操作系统（Windows/Linux/macOS），使用 GoogleTest 作为主要测试框架，通过 CPM 集成。测试文件存放在 `tests/mock/` 下，目录结构镜像原始库结构。删除旧的 `rmdev/test/`、`emdevif/test/`、`RunningPlatforms/`、`TestImplement/`。

单元测试覆盖四种组合：`EMDEVIF_USE_CPP_MODULES=ON/OFF` x `EMDEVIF_ENABLE_EXCEPTIONS=ON/OFF`，通过 4 个 CMake Preset 分别配置和运行。使用 Ninja Multi-Config 构建器，每个 configure preset 对应 Debug 和 Release 两套构建。

## 测试范围

### 纳入单元测试的模块

| 库 | 模块 | 测试方式 |
|---|---|---|
| emdevif_core | data_container (ArrayMap, RingBuffer, FixedString, MessageQueue) | GoogleTest + add_test |
| emdevif_core | resource_guard (Defer, InitGuard, LockGuard, TryFinally) | GoogleTest + add_test |
| emdevif_core | type_traits (misc, tuple_style) | GoogleTest + add_test |
| emdevif_core | utils (BitInt, InitOnce, When) | GoogleTest + add_test |
| emdevif_core | endian, concepts, integer_suffix | GoogleTest + add_test |
| emdevif_core | error_handler, fatal_handler | cmake add_test（独立 executable，不使用 GoogleTest） |
| emdevif_logger | logger | cmake add_test（独立 executable，不使用 GoogleTest） |
| emdevif_timeline | timeline | GoogleTest + add_test（手动递增模拟时钟源） |
| emdevif_peripheral | peripheral_handle_map, peripheral_model | cmake add_test（独立 executable，不使用 GoogleTest；注册函数使用模拟实现） |
| rmdev | control_algorithm (PID) | GoogleTest + add_test |
| rmdev | kinematic_solution (OmniWheelInvSolver, FourWheelChassisSolver) | GoogleTest + add_test |
| rmdev | message_manager (Topic, Subscriber) | GoogleTest + add_test |
| rmdev | debug_assistance (VOFA) | GoogleTest + add_test（验证数据帧格式，不连串口） |

### 排除的模块（不做单元测试）

| 库 | 模块 | 排除原因 |
|---|---|---|
| emdevif_system | 全部 | 依赖 RTOS（FreeRTOS） |
| rmdev | math | 用户指定排除 |
| rmdev | drivers (BMI088, DJIMotor) | 依赖硬件 |
| rmdev | ins | 依赖 CMSIS-DSP |
| rmdev | device_model | 依赖硬件抽象 |

## 目录结构变更

### 删除

```
rmdev/test/                           # 旧 rmdev 测试
emdevif_collection/emdevif/test/      # 旧 emdevif 测试（含 emdevif_test_framework、heap_usage_checker）
RunningPlatforms/                     # 板级工程（STM32/ESP32）
TestImplement/                        # 跨平台测试入口适配
```

### 新建

```
tests/
├── CMakeLists.txt                    # 顶层：根据 preset 参数分发到 mock/
└── mock/
    ├── host_test_pch.hpp             # PCH（#include gtest/gtest.h 等公共头文件）
    ├── CMakeLists.txt                # 集成 GoogleTest（CPM v1.17.0），定义公共编译选项
    │                                 #   创建唯一的 host_gtest 可执行目标
    │                                 #   处理 error_handler / logger / peripheral 的 add_test 子目录
    ├── emdevif/
    │   ├── core/
    │   │   ├── data_container/
    │   │   │   ├── ArrayMap_test.cpp
    │   │   │   ├── RingBuffer_test.cpp
    │   │   │   ├── FixedString_test.cpp
    │   │   │   └── MessageQueue_test.cpp
    │   │   ├── resource_guard/
    │   │   │   └── resource_guard_test.cpp
    │   │   ├── type_traits/
    │   │   │   └── type_traits_test.cpp
    │   │   ├── utils/
    │   │   │   ├── BitInt_test.cpp
    │   │   │   ├── InitOnce_test.cpp
    │   │   │   └── When_test.cpp
    │   │   ├── error_handler/
    │   │   │   ├── CMakeLists.txt
    │   │   │   └── error_handler_test.cpp
    │   │   ├── endian_test.cpp
    │   │   ├── concepts_test.cpp
    │   │   └── integer_suffix_test.cpp
    │   ├── logger/
    │   │   ├── CMakeLists.txt
    │   │   └── logger_test.cpp
    │   ├── peripheral/
    │   │   ├── CMakeLists.txt
    │   │   ├── peripheral_handle_map_test.cpp
    │   │   └── peripheral_model_test.cpp
    │   └── timeline/
    │       └── timeline_test.cpp
    └── rmdev/
        ├── control_algorithm/
        │   └── PID_test.cpp
        ├── kinematic_solution/
        │   └── chassis/
        │       ├── OmniWheelInvSolver_test.cpp
        │       └── FourWheelChassisSolver_test.cpp
        ├── message_manager/
        │   ├── Topic_test.cpp
        │   └── Subscriber_test.cpp
        └── debug_assistance/
            └── VOFA_test.cpp
```

## 关键实现变更

### 1. 根 CMakeLists.txt 重构

- 新增 `RMDEV_HOST_UNIT_TEST` option（默认 OFF），开启时进入宿主单元测试模式
- 宿主测试模式下：不再进入 `RunningPlatforms/`，仅 `add_subdirectory(tests)`
- `PLATFORM_NAME` 为空且 `RMDEV_HOST_UNIT_TEST=ON` 时走宿主测试路径
- 宿主测试路径不需要 `BOARD_NAME`
- 保留 `cmake/get_cpm.cmake` 和 `cmake/utils.cmake` 的引入

### 2. tests/mock/CMakeLists.txt — GoogleTest 集成与测试注册

- 通过 CPM 引入 GoogleTest：`google/googletest`，版本 `v1.17.0`
- 统一使用 `add_test` 注册测试（不使用 `gtest_discover_tests`）
- 创建**唯一**的 GoogleTest 可执行目标 `host_gtest`：
  - `add_executable(host_gtest)`，所有 GoogleTest 模块的测试源文件直接添加到该目标
  - 统一设置预编译头：`target_precompile_headers(host_gtest PRIVATE host_test_pch.hpp)`
  - `target_link_libraries(host_gtest PRIVATE GTest::gtest_main emdevif_core emdevif_logger rmdev_control_algorithm rmdev_kinematic_solution rmdev_message_manager rmdev_debug_assistance ...)`
  - `add_test(NAME host_gtest COMMAND host_gtest)` 注册到 CTest
  - 所有 GoogleTest 测试用例编译到同一个可执行文件，运行一次即可执行全部用例
- 设置 CTest 依赖：`set_tests_properties(host_gtest PROPERTIES DEPENDS emdevif_core_error_handler_test)`
  - `host_gtest` 只有在 `error_handler` 测试通过后才执行
- 对于不使用 GoogleTest 的模块（error_handler、logger、peripheral）：`add_subdirectory` 进入各自目录，各目录内建独立的 `add_test`
- 所有 GoogleTest 测试源文件首行 `#include "host_test_pch.hpp"`

### 3. 预编译头（PCH）机制

文件 `tests/mock/host_test_pch.hpp`：

```cpp
#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
// 按需补充其他公共头文件
```

- CMake 中为 `host_gtest` 设置：
  `cmake
  target_precompile_headers(host_gtest PRIVATE host_test_pch.hpp)
  `
- 每个测试源文件显式 `#include "host_test_pch.hpp"`，保证不使用 PCH 时也能正常编译（回退到普通 include）

### 4. 四种组合的 CMake Preset

使用 **Ninja Multi-Config** 构建器。预设名采用 **PascalCase** 风格。

先定义一个隐藏的 `Default` configure preset 存放通用配置，其余 preset 继承它：

| Preset 名 | 类型 | EMDEVIF_USE_CPP_MODULES | EMDEVIF_ENABLE_EXCEPTIONS |
|---|---|---|---|
| `Default` | configure（hidden） | 不设（由子 preset 覆盖） | 不设（由子 preset 覆盖） |
| `HostTestModulesExceptions` | configure | ON | ON |
| `HostTestModulesNoexceptions` | configure | ON | OFF |
| `HostTestHeadersExceptions` | configure | OFF | ON |
| `HostTestHeadersNoexceptions` | configure | OFF | OFF |

`Default` preset 配置：
- `generator`: Ninja Multi-Config
- `binaryDir`: `/cmake-build-host-test-`
- 公共 cache 变量：`RMDEV_HOST_UNIT_TEST=ON` 等

每个 configure preset 自动对应 Debug / Release 两种构建配置，无需单独创建 build preset。

构建命令示例：

```Bash
# 配置
cmake --preset HostTestModulesExceptions

# Debug 构建并运行
cmake --build --preset HostTestModulesExceptions --config Debug
ctest --preset HostTestModulesExceptions -C Debug

# Release 构建并运行
cmake --build --preset HostTestModulesExceptions --config Release
ctest --preset HostTestModulesExceptions -C Release
```

### 5. 异常与 `GTEST_SKIP` 策略

涉及异常的测试用例使用 `#ifdef __cpp_exceptions` 检测异常支持：

```cpp
TEST(SomeTest, ExceptionDependentCase) {
#ifdef __cpp_exceptions
    // 依赖异常的测试逻辑
    EXPECT_THROW(..., std::exception);
#else
    GTEST_SKIP() << "Exceptions are disabled (EMDEVIF_ENABLE_EXCEPTIONS=OFF)";
#endif
}
```

- 仅对**直接依赖 C++ 异常机制**的测试用例使用 `GTEST_SKIP`
- 不依赖异常的测试用例正常执行，无需条件编译
- 保证关闭异常的预设下所有测试仍然可编译、可运行，仅跳过异常相关用例

### 6. 复杂模块：error_handler / logger / peripheral 的 add_test

不使用 GoogleTest，各自目录有独立 `CMakeLists.txt`：

- 编译为独立 executable，手动编写 `main()` 函数，内联断言或使用 `assert()`
- `add_test(NAME ... COMMAND ...)` 注册到 CTest
- error_handler 测试名称：`emdevif_core_error_handler_test`（供 `host_gtest` 的 `DEPENDS` 引用）
- error_handler：mock `terminateFunction` 和 `fatalHandlerCallback`，验证注册和调用行为
- logger：配置 `emdevif_user_declares` 的 logger buffer，mock 输出后端，捕获并比对输出
- peripheral：模拟注册函数（如 peripheral_handle_map 的注册/查找回调），验证句柄映射逻辑和外设模型接口

### 7. 测试执行顺序

CTest 依次执行：
1. `emdevif_core_error_handler_test`（error_handler 独立 executable）
2. `emdevif_logger_test`（logger 独立 executable）
3. `emdevif_peripheral_test`（peripheral 独立 executable）
4. `host_gtest`（仅在 error_handler 测试通过后执行，通过 `DEPENDS` 保证）

logger 和 peripheral 测试可与 host_gtest **并行**执行（无 DEPENDS 约束），但 error_handler 必须先通过。

### 8. timeline mock 时钟源

手动递增计数器实现 mock 时钟：

```cpp
// 测试用模拟时钟
static std::uint64_t mock_timestamp = 0;

// 注入到被测模块的时钟获取函数
std::uint64_t mockGetTimestamp() {
    return mock_timestamp++;
}

TEST(TimelineTest, TimestampIncrement) {
    auto t1 = mockGetTimestamp();
    auto t2 = mockGetTimestamp();
    EXPECT_GT(t2, t1);
    EXPECT_EQ(t2 - t1, 1);
}
```

具体 mock 注入方式取决于 `emdevif_timeline` 的接口设计（宏替换、链接期替换或依赖注入）。

### 9. 测试迁移策略

- 有旧测试的模块：将 `emdevif_test_framework` 宏（`TEST_SUIT`、`TEST_CASE_BEGIN`、`EXPECT_*`、`ASSERT_*` 等）改写为 GoogleTest 的 `TEST` / `TEST_F` / `EXPECT_*` / `ASSERT_*`
- 保留旧测试中的核心断言逻辑和数据，仅替换框架层
- 新模块（无旧测试）：从零编写 GoogleTest 测试
- C++20 Modules 路径下：测试文件使用 `import` 导入被测模块；Header 路径下：使用 `#include`，根据 `EMDEVIF_USE_MODULES` 宏条件编译
- 所有 GoogleTest 测试源文件首行 `#include "host_test_pch.hpp"`

### 10. emdevif 和 rmdev 子模块变更

- `emdevif/CMakeLists.txt`：移除 `EMDEVIF_ENABLE_TESTS` 相关逻辑（`add_subdirectory(test)` 分支及其上游 `enable_testing()` 调用）
- `rmdev/CMakeLists.txt`：移除 `RMDEV_ENABLE_TESTS` 相关逻辑（`add_subdirectory(test)` 分支）
- `emdevif/AGENTS.md`、`rmdev/AGENTS.md`：更新测试相关描述，指向 `tests/mock/` 新测试路径

### 11. 根 AGENTS.md 更新

- 移除 `RunningPlatforms/`、`TestImplement/` 相关说明
- 移除 `testInit` / `testEntry` 契约
- 新增 `tests/mock/` 测试架构说明
- 新增宿主单元测试构建命令和 Preset 表

## 测试用例覆盖要求

### 迁移的旧测试（已有用例，改写框架层）

- ArrayMap：构造、插入/删除、erase_if、自定义 KeyEqual
- RingBuffer：基本读写、满/空判断、覆盖写
- resource_guard：Defer 执行、InitGuard 状态、LockGuard RAII、TryFinally
- type_traits：类型萃取编译期断言
- BitInt：有符号/无符号运算、异常/无异常路径（异常路径用 `GTEST_SKIP`）、溢出行为
- InitOnce：单次初始化保证
- When：条件分支覆盖
- endian：字节序转换
- OmniWheelInvSolver：运动学逆解算正确性
- Topic：发布订阅、多订阅者、push/peek
- Subscriber：订阅者生命周期

### 新增测试（从零编写）

- FixedString：构造、拼接、比较、容量边界
- MessageQueue：push/pop、满/空、移动语义
- concepts：编译期概念检查（static_assert 为主）
- integer_suffix：字面量正确类型推导
- timeline：时间戳递增、差值计算（手动递增 mock 时钟源）
- PID：比例/积分/微分项计算、积分饱和、输出限幅
- FourWheelChassisSolver：四轮底盘正/逆解算
- VOFA：justfloat 协议数据帧格式正确性（不连串口，仅验证二进制输出）
- peripheral_handle_map：模拟注册/查找回调函数，验证句柄映射正确性
- peripheral_model：验证各外设模型接口的调用行为（使用模拟实现）
- error_handler：mock terminateFunction / fatalHandlerCallback，验证注册与调用
- logger：mock 输出后端，配置 user_declares buffer，捕获日志输出并与预期比对

## 构建与运行

```Bash
# 列出所有可用预设
cmake --list-presets

# 配置（4 选 1）
cmake --preset HostTestModulesExceptions
cmake --preset HostTestHeadersNoexceptions
# ...

# Debug 构建并运行测试
cmake --build --preset HostTestModulesExceptions --config Debug
ctest --preset HostTestModulesExceptions -C Debug

# Release 构建并运行测试
cmake --build --preset HostTestModulesExceptions --config Release
ctest --preset HostTestModulesExceptions -C Release
```

## 假设与约定

1. **四种组合通过 4 次独立 cmake 配置实现**，不要求同次 cmake 内编译 4 个 target
2. **Ninja Multi-Config**：一个 configure preset 同时支持 Debug 和 Release 构建
3. **预设命名**：`HostTest{Modules|Headers}{Exceptions|Noexceptions}`，使用 PascalCase 风格
4. **隐藏 Default preset**：存放通用配置（generator、binaryDir、公共 cache 变量），其余 preset 继承
5. **GoogleTest 版本**：`v1.17.0`，通过 CPM 引入（沿用仓库现有 `cmake/get_cpm.cmake`）
6. **不使用 `gtest_discover_tests`**：统一使用 CMake 原生 `add_test` 注册测试
7. **唯一 GoogleTest 可执行目标**：所有 GoogleTest 测试源文件编译到同一个 `host_gtest` 可执行文件，只链接一次 `GTest::gtest_main`
8. **CTest 依赖**：`host_gtest` 的 `DEPENDS` 设为 `emdevif_core_error_handler_test`，error_handler 测试通过后才执行 gtest
9. **预编译头**：`host_test_pch.hpp` 集中引入 GoogleTest 头文件；CMake 中为 `host_gtest` 设置 PCH，测试源文件显式 `#include "host_test_pch.hpp"`，保证无 PCH 也可正常编译
10. **异常处理**：依赖异常的测试用例通过 `#ifdef __cpp_exceptions` + `GTEST_SKIP` 在关闭异常的预设中跳过
11. **timeline mock**：手动递增计数器实现，不依赖 `std::chrono`
12. **error_handler / logger / peripheral 不使用 GoogleTest**：编译为独立 executable，cmake `add_test` 注册；peripheral 使用模拟函数替代真实硬件注册
13. **旧 `emdevif_test_framework` 子模块**：随 `emdevif/test/` 目录一起删除
14. **emdevif_system、rmdev math/drivers/ins/device_model** 明确排除，不编写单元测试
15. **集成测试（单片机端）**不在本次计划范围内，后续单独规划设计