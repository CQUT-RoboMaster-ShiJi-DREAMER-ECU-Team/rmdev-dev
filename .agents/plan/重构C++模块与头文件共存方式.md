# 重构计划：C++ 模块与头文件共存方式

## 目标

将当前的"宏嵌入导出"模式（通过 `EMDEVIF_MODULE_EXPORT` 在头文件中嵌入 `export` 关键字）重构为"隐式包含"模式（头文件为纯 C++ 代码且无条件包含所有依赖，`.cppm` 中使用 `export namespace { using }` 选择性导出符号）。

## 模式对比

### 当前模式（boost_pfr 风格）

```
// lib.hpp
#include "config.hpp"                    // 提供 EMDEVIF_MODULE_EXPORT 宏
#ifndef EMDEVIF_MODULE_INTERFACE_UNIT
  #include <cstddef>                     // 标准库头文件有条件包含
  #include "other_project.hpp"           // 项目头文件有条件包含
#endif
EMDEVIF_MODULE_EXPORT                    // 在模块模式下展开为 export
namespace lib { /* 声明 */ }

// lib.cppm
module;
#include <cstddef>                       // 在全局模块段单独包含标准库头文件
#define EMDEVIF_MODULE_INTERFACE_UNIT    // 触发头文件中的 export 宏
export module lib;
import other.module;
#include "lib.hpp"                       // 头文件在模块管辖范围内被包含
```

### 新模式（隐式包含）

```
// lib.hpp
#pragma once
#ifndef LIB_HPP
#define LIB_HPP
#include <cstddef>                       // 标准库头文件无条件包含
#include "other_project.hpp"             // 项目头文件无条件包含（隐式）
namespace lib { /* 声明，无 export 宏 */ }
#endif

// lib.cppm
module;
#include "lib.hpp"                       // 头文件在全局模块段被包含
export module lib;
// 无需 import 项目模块，依赖已由头文件隐式包含
export namespace lib {                   // 选择性导出
    using ::lib::func;
    using ::lib::Type;
}
```

### 关键区别

| 方面 | 当前模式 | 新模式 |
|------|---------|--------|
| 头文件中的 export | 通过 `EMDEVIF_MODULE_EXPORT` 宏嵌入 | 无 export，头文件是纯 C++ |
| 头文件中标准库 `#include` | 被 `#ifndef EMDEVIF_MODULE_INTERFACE_UNIT` 守卫 | 无条件包含 |
| 头文件中项目头文件 `#include` | 被守卫 | 无条件包含（隐式） |
| `#include "lib.hpp"` 位置 | 在 `export module` 之后（模块管辖范围） | 在 `export module` 之前（全局模块段） |
| `.cppm` 中项目依赖导入 | `import other.module` | 不存在，依赖由头文件隐式包含 |
| 导出控制 | 头文件中标记 `EMDEVIF_MODULE_EXPORT` | `.cppm` 中 `using` 声明选择性导出 |
| `detail` 命名空间 | 不标记 `EMDEVIF_MODULE_EXPORT` | 不在 `export namespace` 中列出 |
| `config.hpp` | 定义 `EMDEVIF_MODULE_EXPORT` 等宏和 `EMDEVIF_MODULE_INTERFACE_UNIT` | 不再需要 export 相关宏 |

## 变更范围统计

| 子模块 | .hpp 文件 | .cppm 文件 | .inl 文件 | .cpp 文件 |
|--------|----------|-----------|----------|----------|
| emdevif | ~60 | 41 | 6 | 7 |
| rmdev | ~25 | 14 | 0 | 1 |
| emdevif_stm32_peripheral | 6 | 5 | 0 | 0 |
| **合计** | **~91** | **60** | **6** | **8** |

另有：
- 1 个 `config.hpp` 需简化
- 1 个 `SKILL.md` 需重写
- 17 个测试 `.cpp` 文件使用 `EMDEVIF_USE_MODULES`（预计无需修改）

---

## 阶段 0：更新技能文档

**文件：** `.agents/skills/cpp-lib-headers-modules-both-supports/SKILL.md`

重写技能文档，描述新模式：

### 0.1 配置头文件

移除 `EMDEVIF_MODULE_EXPORT`、`EMDEVIF_MODULE_EXPORT_BEGIN`、`EMDEVIF_MODULE_EXPORT_END` 宏定义。
移除 `EMDEVIF_MODULE_INTERFACE_UNIT` 相关逻辑。
保留 `EMDEVIF_USE_MODULES` 定义（供 `.cpp` 源文件使用）。

### 0.2 纯头文件库

**头文件（`lib.hpp`）：**

```cpp
#pragma once
#ifndef LIB_HPP
#define LIB_HPP

#include <cstddef>                        // 标准库头文件：无条件包含
#include "lib/b.hpp"                      // 项目模块依赖：无条件包含（隐式）

namespace lib::detail {
    // 不导出的实现细节
}

namespace lib {
    int func(int a) { return a; }         // 无需 export 宏
    struct Type {};
    struct UnExportedType {};             // 不导出
}

#endif  // !LIB_HPP
```

**模块接口单元（`lib.cppm`）：**

```cpp
module;

#include "lib.hpp"                        // 头文件在全局模块段

export module lib;

// 无需 import 项目模块，依赖已由头文件隐式包含

export namespace lib {                    // 选择性导出
    using ::lib::func;
    using ::lib::Type;
    // UnExportedType 不列出，不导出
}
```

### 0.3 组件依赖模式

如果 `lib/a.hpp` 依赖 `lib/b.hpp`，且两者都提供模块：

**`lib/a.hpp`：**

```cpp
#pragma once
#ifndef LIB_A_HPP
#define LIB_A_HPP

#include <cstddef>
#include "lib/b.hpp"                      // 无条件包含（隐式）

namespace lib {
    void funcA();
}

#endif
```

**`lib.a.cppm`：**

```cpp
module;

#include "lib/a.hpp"

export module lib.a;

// 无需 import lib.b，依赖已由头文件隐式包含

export namespace lib {
    using ::lib::funcA;
}
```

### 0.4 存在源文件的库

**头文件**：与纯头文件库模式相同。

**源文件（`lib.cpp`）：** 与当前模式相同，使用 `EMDEVIF_USE_MODULES` 进行条件编译：

```cpp
#if (defined(EMDEVIF_USE_MODULES) && EMDEVIF_USE_MODULES)
module;
#include <cstddef>                        // 全局模块段包含标准库头文件
module lib;                               // 模块实现单元
import emdevif.core.error_handler;        // import 替代 #include
#else
#include "lib.hpp"                        // 头文件模式
#endif

// 实现代码...
```

### 0.5 隐式包含规则总结

在 `.hpp` 文件中，所有 `#include` 均无条件包含，不再使用 `EMDEVIF_MODULE_INTERFACE_UNIT` 守卫。
`.cppm` 文件不再 `import` 项目模块，项目依赖完全由头文件的 `#include` 隐式提供。

---

## 阶段 1：重构 emdevif 子模块

### 1.1 简化 `config.hpp`

**文件：** `emdevif/core/inc/emdevif/core/detail/config.hpp`

移除 `EMDEVIF_MODULE_EXPORT`、`EMDEVIF_MODULE_EXPORT_BEGIN`、`EMDEVIF_MODULE_EXPORT_END` 宏定义。
移除 `EMDEVIF_MODULE_INTERFACE_UNIT` 相关逻辑。
保留 `EMDEVIF_USE_MODULES` 默认值定义。

新内容：

```cpp
#pragma once
#ifndef EMDEVIF_CORE_DETAIL_CONFIG_HPP
    #define EMDEVIF_CORE_DETAIL_CONFIG_HPP

    #ifndef EMDEVIF_USE_MODULES
        #define EMDEVIF_USE_MODULES 0
    #endif

#endif  // !EMDEVIF_CORE_DETAIL_CONFIG_HPP
```

### 1.2 重构 emdevif `.hpp` 文件

对每个 `.hpp` 文件执行以下变更：

1. **移除** `#include "emdevif/core/detail/config.hpp"`（不再需要 export 宏）
2. **移除** 所有 `EMDEVIF_MODULE_EXPORT`、`EMDEVIF_MODULE_EXPORT_BEGIN`、`EMDEVIF_MODULE_EXPORT_END` 使用
3. **移除** 所有 `#ifndef EMDEVIF_MODULE_INTERFACE_UNIT` / `#endif` 守卫块，使所有 `#include` 成为无条件包含
4. **移除** `#define EMDEVIF_MODULE_INTERFACE_UNIT` 的任何引用

不再需要守卫，所有 `#include` 均为无条件包含。

### 1.3 重构 emdevif `.inl` 文件

对每个 `.inl` 文件执行以下变更：

1. **移除** 所有 `EMDEVIF_MODULE_EXPORT`、`EMDEVIF_MODULE_EXPORT_BEGIN`、`EMDEVIF_MODULE_EXPORT_END` 使用

受影响的 `.inl` 文件：
- `emdevif/system/Implements/FreeRTOS/inc/emdevif/system_impl/thread.inl`
- `emdevif/system/Implements/FreeRTOS/inc/emdevif/system_impl/sys_queue.inl`
- `emdevif/system/Implements/FreeRTOS/inc/emdevif/system_impl/semaphore.inl`（2 处）
- `emdevif/system/Implements/FreeRTOS/inc/emdevif/system_impl/heap.inl`
- `emdevif/system/Implements/FreeRTOS/inc/emdevif/system_impl/mutex.inl`
- `emdevif/system/Implements/FreeRTOS/inc/emdevif/system_impl/event_group_implements.inl`（含 `BEGIN`/`END` 块）

以及 rmdev 中的：
- `rmdev/modules/rmdev_math/inc/rmdev/matrix/detail/arm_matrix/ArmMatrix.inl`

### 1.4 重构 emdevif `.cppm` 文件

对每个 `.cppm` 文件执行以下变更：

1. **移除** 全局模块段中单独的标准库 `#include`（它们现在由 `.hpp` 包含）
2. **将** `#include "lib.hpp"` 移到 `export module` 之前（全局模块段）
3. **移除** `#define EMDEVIF_MODULE_INTERFACE_UNIT`（不再需要）
4. **移除** clang pragma（`#pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"`），因为标准库头文件现在在全局模块段被包含，不再产生警告
5. **移除** `import` 语句（项目模块依赖已由头文件隐式包含）
6. **添加** `export namespace` 块，包含 `using` 声明，选择性导出符号

#### `.cppm` 新模板

```cpp
/**
 * @file xxx.cppm
 * @brief 描述
 */

module;

#include "path/to/xxx.hpp"

export module emdevif.xxx;

// 无需 import 项目模块，依赖已由头文件隐式包含

export namespace emdevif {
    using ::emdevif::Symbol1;
    using ::emdevif::Symbol2;
}
```

#### 聚合模块（无全局模块段）

聚合模块（如 `utils.cppm`、`resource_guard.cppm`）仅使用 `export import`，无需修改结构：

```cpp
export module emdevif.core.utils;

export import emdevif.core.utils.bit_int;
export import emdevif.core.utils.init_once;
export import emdevif.core.utils.when;
```

这些文件的 `.hpp` 也是聚合头文件（只 `#include` 子头文件），同样无需修改。

### 1.5 确定各模块需要导出的符号

需要为每个 `.cppm` 文件确定 `export namespace` 中的 `using` 声明列表。
原则：导出当前标记了 `EMDEVIF_MODULE_EXPORT` 的所有符号，不导出 `detail` 命名空间和未标记的符号。

**需要特别注意的符号类型：**

| 符号类型 | `using` 声明示例 | 备注 |
|---------|-----------------|------|
| 类型别名 | `using ::emdevif::TerminateFunction;` | 直接 using |
| 函数 | `using ::emdevif::terminate;` | 覆盖所有重载 |
| 类/结构体 | `using ::emdevif::ErrorCode;` | 导出类时，公开成员自动导出 |
| 概念 | `using ::emdevif::HaveLimitType;` | 直接 using |
| 函数模板 | `using ::emdevif::terminateIfNullptr;` | 直接 using（覆盖所有特化） |
| 类模板 | `using ::emdevif::MotorRotateRecorder;` | using 声明引用模板名，无需参数列表 |
| constexpr 变量 | `using ::emdevif::init_fail_max_count;` | 直接 using |
| 用户定义字面量 | `using ::emdevif::literals::integer_literals::operator""_u8;` | 逐个导出 |
| inline 命名空间成员 | `using ::emdevif::literals::integer_literals::operator""_u8;` | 通过完整限定路径 |

### 1.6 emdevif `.cpp` 源文件

7 个 `.cpp` 源文件使用 `EMDEVIF_USE_MODULES` 进行条件编译。这些文件的结构不需要大幅修改，因为：
- 它们不使用 `EMDEVIF_MODULE_EXPORT`
- 它们的条件编译逻辑（`#if EMDEVIF_USE_MODULES` / `module;` / `import`）与新模式兼容
- 可能需要微调全局模块段中的 `#include`（确保包含所有需要的标准库头文件）

受影响的文件：
1. `emdevif/core/src/error_handler/error_handler.cpp`
2. `emdevif/core/src/error_handler/fatal_handler_extern_c.cpp`
3. `emdevif/logger/src/logger.cpp`
4. `emdevif/system/Implements/FreeRTOS/thread.cpp`
5. `emdevif/system/Implements/FreeRTOS/semaphore.cpp`
6. `emdevif/system/Implements/FreeRTOS/mutex.cpp`
7. `emdevif/system/Implements/FreeRTOS/event_group.cpp`

---

## 阶段 2：重构 rmdev 子模块

### 2.1 重构 rmdev `.hpp` 文件

与阶段 1.2 相同的变更规则。rmdev 没有 `config.hpp`，所有 `.hpp` 文件需要：

1. **移除** `#include "emdevif/core/detail/config.hpp"`
2. **移除** 所有 `EMDEVIF_MODULE_EXPORT` 使用
3. **移除** 所有 `#ifndef EMDEVIF_MODULE_INTERFACE_UNIT` / `#endif` 守卫块，使所有 `#include` 成为无条件包含
4. **移除** `#define EMDEVIF_MODULE_INTERFACE_UNIT` 的任何引用

受影响的 rmdev `.hpp` 文件列表：

**rmdev_math：**
- `rmdev/modules/rmdev_math/inc/rmdev/math.hpp`（聚合）
- `rmdev/modules/rmdev_math/inc/rmdev/math/basic.hpp`
- `rmdev/modules/rmdev_math/inc/rmdev/math/const_value.hpp`
- `rmdev/modules/rmdev_math/inc/rmdev/math/decimal.hpp`
- `rmdev/modules/rmdev_math/inc/rmdev/math/range.hpp`
- `rmdev/modules/rmdev_math/inc/rmdev/math/trigonometric.hpp`
- `rmdev/modules/rmdev_math/inc/rmdev/matrix.hpp`（聚合）
- `rmdev/modules/rmdev_math/inc/rmdev/matrix/matrix_base.hpp`
- `rmdev/modules/rmdev_math/inc/rmdev/matrix/detail/arm_matrix/ArmMatrixTraits.hpp`
- `rmdev/modules/rmdev_math/inc/rmdev/matrix/detail/arm_matrix/ArmMatrix.hpp`
- `rmdev/modules/rmdev_math/inc/rmdev/matrix/detail/arm_matrix/ArmMatrix.inl`

**rmdev_message_manager：**
- `rmdev/modules/rmdev_message_manager/inc/rmdev/message_manager.hpp`（聚合）
- `rmdev/modules/rmdev_message_manager/inc/rmdev/message_manager/topic.hpp`
- `rmdev/modules/rmdev_message_manager/inc/rmdev/message_manager/subscriber.hpp`

**rmdev_control_algorithm：**
- `rmdev/modules/rmdev_control_algorithm/inc/rmdev/control_algorithm/pid.hpp`

**rmdev_device_model：**
- `rmdev/modules/rmdev_device_model/inc/rmdev/device_model/motor.hpp`
- `rmdev/modules/rmdev_device_model/inc/rmdev/device_model/sensor/imu.hpp`

**rmdev_kinematic_solution：**
- `rmdev/modules/rmdev_kinematic_solution/inc/rmdev/kinematic_solution/chassis/OmniWheelInvSolver.hpp`
- `rmdev/modules/rmdev_kinematic_solution/inc/rmdev/kinematic_solution/chassis/FourWheelChassisSolver.hpp`

**rmdev_debug_assistance：**
- `rmdev/modules/rmdev_debug_assistance/inc/rmdev/debug_assistance/vofa.hpp`

**rmdev_ins：**
- `rmdev/modules/rmdev_ins/inc/rmdev/ins.hpp`（聚合）
- `rmdev/modules/rmdev_ins/inc/rmdev/ins/ins_base.hpp`
- `rmdev/modules/rmdev_ins/inc/rmdev/ins/detail/quaternion_ekf_ins/QuaternionEKF_INS.hpp`

**rmdev_driver_DJIMotor：**
- `rmdev/drivers/rmdev_driver_DJIMotor/inc/rmdev/driver/dji_motor.hpp`（聚合）
- `rmdev/drivers/rmdev_driver_DJIMotor/inc/rmdev/driver/dji_motor/can_address.hpp`
- `rmdev/drivers/rmdev_driver_DJIMotor/inc/rmdev/driver/dji_motor/class_dji_motor.hpp`
- `rmdev/drivers/rmdev_driver_DJIMotor/inc/rmdev/driver/dji_motor/dji_motor_group.hpp`

**rmdev_driver_BMI088：**
- `rmdev/drivers/rmdev_driver_BMI088/inc/rmdev/driver/bmi088.hpp`

### 2.2 重构 rmdev `.cppm` 文件

与阶段 1.4 相同的变更规则。

受影响的 14 个 `.cppm` 文件：
1. `rmdev/modules/rmdev_math/math.cppm`
2. `rmdev/modules/rmdev_math/Matrix.cppm`
3. `rmdev/modules/rmdev_message_manager/message_manager.cppm`（聚合，`export import`）
4. `rmdev/modules/rmdev_message_manager/Topic.cppm`
5. `rmdev/modules/rmdev_message_manager/Subscriber.cppm`
6. `rmdev/modules/rmdev_control_algorithm/modules/pid.cppm`
7. `rmdev/modules/rmdev_device_model/motor/Motor.cppm`
8. `rmdev/modules/rmdev_device_model/sensor/Imu.cppm`
9. `rmdev/modules/rmdev_kinematic_solution/chassis/OmniWheelInvSolver.cppm`
10. `rmdev/modules/rmdev_kinematic_solution/chassis/FourWheelChassisSolver.cppm`
11. `rmdev/modules/rmdev_debug_assistance/vofa.cppm`
12. `rmdev/modules/rmdev_ins/ins.cppm`
13. `rmdev/drivers/rmdev_driver_DJIMotor/dji_motor.cppm`
14. `rmdev/drivers/rmdev_driver_BMI088/Bmi088.cppm`

### 2.3 rmdev `.cpp` 源文件

1 个文件：`rmdev/modules/rmdev_ins/src/quaternion_ekf_ins/kalman_filter.cpp`
预计无需修改（其条件编译逻辑与新模式兼容）。

---

## 阶段 3：重构 emdevif_stm32_peripheral 子模块

### 3.1 重构 `.hpp` 文件

6 个 `.hpp` 文件：
1. `emdevif_stm32_peripheral/inc/emdevif/stm32_peripheral/hal/usart.hpp`
2. `emdevif_stm32_peripheral/inc/emdevif/stm32_peripheral/hal/spi.hpp`
3. `emdevif_stm32_peripheral/inc/emdevif/stm32_peripheral/hal/pwm.hpp`
4. `emdevif_stm32_peripheral/inc/emdevif/stm32_peripheral/hal/gpio.hpp`
5. `emdevif_stm32_peripheral/inc/emdevif/stm32_peripheral/hal/can.hpp`
6. `emdevif_stm32_peripheral/inc/emdevif/stm32_peripheral/hal/detail/hal_status_mapper.hpp`

注意 `pwm.hpp` 使用了逐声明的 `EMDEVIF_MODULE_EXPORT`（struct PwmHandle 和三个函数），需要在 `.cppm` 的 `export namespace` 中分别列出。

### 3.2 重构 `.cppm` 文件

5 个 `.cppm` 文件：
1. `emdevif_stm32_peripheral/STM32_HAL_Driver/usart_hal_impl.cppm`
2. `emdevif_stm32_peripheral/STM32_HAL_Driver/spi_hal_impl.cppm`
3. `emdevif_stm32_peripheral/STM32_HAL_Driver/pwm_hal_impl.cppm`
4. `emdevif_stm32_peripheral/STM32_HAL_Driver/gpio_hal_impl.cppm`
5. `emdevif_stm32_peripheral/STM32_HAL_Driver/can_hal_impl.cppm`

这些文件的 `.cppm` 包含 CubeMX 生成的头文件（如 `"usart.h"`、`"can.h"`），这些头文件应在全局模块段无条件包含。

---

## 阶段 4：验证

### 4.1 构建验证

使用全部 4 个预设进行构建和测试：

```bash
# 1. Modules + Exceptions
cmake --preset HostTestModulesExceptions
cmake --build --preset HostTestModulesExceptions --config Debug
ctest --preset HostTestModulesExceptions -C Debug
cmake --build --preset HostTestModulesExceptions --config Release
ctest --preset HostTestModulesExceptions -C Release

# 2. Modules + No Exceptions
cmake --preset HostTestModulesNoexceptions
cmake --build --preset HostTestModulesNoexceptions --config Debug
ctest --preset HostTestModulesNoexceptions -C Debug

# 3. Headers + Exceptions
cmake --preset HostTestHeadersExceptions
cmake --build --preset HostTestHeadersExceptions --config Debug
ctest --preset HostTestHeadersExceptions -C Debug

# 4. Headers + No Exceptions
cmake --preset HostTestHeadersNoexceptions
cmake --build --preset HostTestHeadersNoexceptions --config Debug
ctest --preset HostTestHeadersNoexceptions -C Debug
```

### 4.2 验证重点

1. **Modules ON 路径**：确保 `export namespace { using }` 正确导出符号，`import` 可以获取所需符号
2. **Modules OFF 路径**：确保头文件直接 `#include` 可以正常工作
3. **异常开关**：确保 `TEST_ENABLE_EXCEPTIONS` 仍能正确控制
4. **detail 命名空间**：确保不导出的符号在模块模式下不可见
5. **隐式包含依赖**：确保 `#include` 链完整，所有依赖通过头文件隐式覆盖

---

## 风险评估

### 高风险

1. **编译器兼容性**：`export namespace { using }` 模式在不同编译器（MSVC、GCC、Clang）上的支持程度可能不同。需要逐一测试。
2. **类模板导出**：`using` 声明对类模板的处理可能有编译器差异。
3. **用户定义字面量**：`operator""_u8` 等 `using` 声明的语法可能需要特殊处理。

### 中风险

4. **内联命名空间**：`emdevif::inline literals::inline integer_literals` 的 `using` 声明可能需要特殊语法。
5. **重载函数**：`fatalHandler` 的多个重载版本，单个 `using` 声明应覆盖所有重载，但需验证。
6. **宏头文件**：移除 `config.hpp` 包含后，某些依赖 `EMDEVIF_USE_MODULES` 的代码可能受影响。

### 低风险

7. **聚合模块**：`utils.cppm` 等纯 `export import` 模块无需修改。
8. **测试文件**：17 个测试 `.cpp` 文件使用 `#if EMDEVIF_USE_MODULES` 切换 `import`/`#include`，预计无需修改。

---

## 执行顺序

由于子模块间存在依赖关系（emdevif 是基础库），建议按以下顺序执行：

1. **阶段 0**：更新 SKILL.md（纯文档，不影响构建）
2. **阶段 1**：重构 emdevif（基础库，无项目内部依赖）
   - 1.1 简化 config.hpp
   - 1.2 重构 .hpp 文件
   - 1.3 重构 .inl 文件
   - 1.4 重构 .cppm 文件
   - 1.5 验证 emdevif 构建
3. **阶段 2**：重构 rmdev（依赖 emdevif）
   - 2.1 重构 .hpp 文件
   - 2.2 重构 .cppm 文件
   - 2.3 验证 rmdev 构建
4. **阶段 3**：重构 emdevif_stm32_peripheral（依赖 emdevif）
   - 3.1 重构 .hpp 文件
   - 3.2 重构 .cppm 文件
   - 3.3 验证构建
5. **阶段 4**：全量验证（4 个预设）

每个子模块内部，建议先修改 `.hpp` 和 `.inl` 文件，再修改 `.cppm` 文件，确保修改是原子性的（同一个子模块的所有变更一起提交）。

---

## 逐文件变更清单

### emdevif/core

| 文件 | 变更类型 |
|------|---------|
| `core/inc/emdevif/core/detail/config.hpp` | 简化：移除 export 宏 |
| `core/inc/emdevif/core/concepts.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/endian.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/integer_suffix.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/type_traits.hpp` | 聚合，无变更 |
| `core/inc/emdevif/core/type_traits/misc.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/type_traits/tuple_style.hpp` | 移除 BEGIN/END 块，移除所有守卫 |
| `core/inc/emdevif/core/error_handler.hpp` | 聚合，无变更 |
| `core/inc/emdevif/core/error_handler/fatal_handler.hpp` | 移除 export 宏（10处），移除所有守卫 |
| `core/inc/emdevif/core/error_handler/error_code.hpp` | 移除 namespace 级 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/utils.hpp` | 聚合，无变更 |
| `core/inc/emdevif/core/utils/when.hpp` | 移除 BEGIN/END 块和 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/utils/init_once.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/utils/bit_int.hpp` | 聚合，可能无变更 |
| `core/inc/emdevif/core/utils/bit_int/basic_config.hpp` | 移除 export 宏（2处） |
| `core/inc/emdevif/core/utils/bit_int/unsigned_bit_int.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/utils/bit_int/signed_bit_int.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/resource_guard.hpp` | 聚合，无变更 |
| `core/inc/emdevif/core/resource_guard/defer.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/resource_guard/init_guard.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/resource_guard/lock_guard.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/resource_guard/try_finally.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/data_container/ring_buffer.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/data_container/message_queue.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/data_container/fixed_string.hpp` | 移除 export 宏，移除所有守卫 |
| `core/inc/emdevif/core/data_container/array_map.hpp` | 聚合，可能无变更 |
| `core/inc/emdevif/core/data_container/array_map/array_map.hpp` | 移除 export 宏（2处），移除所有守卫 |
| `core/inc/emdevif/core/data_container/array_map/basic_array_map.hpp` | 移除所有守卫 |
| `core/inc/emdevif/core/data_container/array_map/static_map.hpp` | 移除 export 宏，移除所有守卫 |
| `core/modules/concepts.cppm` | 重构：头文件移入全局模块段，添加 export namespace |
| `core/modules/endian.cppm` | 同上 |
| `core/modules/integer_suffix.cppm` | 同上 |
| `core/modules/type_traits.cppm` | 同上 |
| `core/modules/error_handler.cppm` | 同上 |
| `core/modules/utils.cppm` | 聚合模块，无结构变更 |
| `core/modules/utils/when.cppm` | 重构 |
| `core/modules/utils/InitOnce.cppm` | 重构 |
| `core/modules/utils/BitInt.cppm` | 重构 |
| `core/modules/resource_guard.cppm` | 聚合模块，无结构变更 |
| `core/modules/resource_guard/defer.cppm` | 重构 |
| `core/modules/resource_guard/init_guard.cppm` | 重构 |
| `core/modules/resource_guard/lock_guard.cppm` | 重构 |
| `core/modules/resource_guard/try_finally.cppm` | 重构 |
| `core/modules/data_container/ring_buffer.cppm` | 重构 |
| `core/modules/data_container/message_queue.cppm` | 重构 |
| `core/modules/data_container/fixed_string.cppm` | 重构 |
| `core/modules/data_container/array_map.cppm` | 重构 |
| `core/src/error_handler/error_handler.cpp` | 检查，可能微调 |
| `core/src/error_handler/fatal_handler_extern_c.cpp` | 检查，可能微调 |

### emdevif/logger

| 文件 | 变更类型 |
|------|---------|
| `logger/inc/emdevif/logger.hpp` | 聚合，条件包含子头文件，无 export 宏，需检查 |
| `logger/inc/emdevif/logger/config.hpp` | 包含 emdevif config，可能需微调 |
| `logger/inc/emdevif/logger/config_values.hpp` | 移除 export 宏 |
| `logger/inc/emdevif/logger/sync_async_interface.hpp` | 移除 export 宏，移除所有守卫 |
| `logger/inc/emdevif/logger/sync.hpp` | 移除守卫，可能需重构 |
| `logger/inc/emdevif/logger/async.hpp` | 移除守卫，可能需重构 |
| `logger/inc/emdevif/logger/external_impl.hpp` | 移除 export 宏，移除所有守卫 |
| `logger/logger.cppm` | 重构 |
| `logger/src/logger.cpp` | 检查，可能微调 |

### emdevif/timeline

| 文件 | 变更类型 |
|------|---------|
| `timeline/inc/emdevif/timeline.hpp` | 移除 export 宏，移除所有守卫 |
| `timeline/timeline.cppm` | 重构 |

### emdevif/system

| 文件 | 变更类型 |
|------|---------|
| `system/inc/emdevif/system/atomic.hpp` | 移除 export 宏，移除所有守卫 |
| `system/inc/emdevif/system/sys_message_slot.hpp` | 移除 export 宏，移除所有守卫 |
| `system/inc/emdevif/system/thread.hpp` | 移除 export 宏，移除所有守卫 |
| `system/inc/emdevif/system/sys_queue.hpp` | 移除 export 宏，移除所有守卫 |
| `system/inc/emdevif/system/semaphore.hpp` | 移除 export 宏，移除所有守卫 |
| `system/inc/emdevif/system/mutex.hpp` | 移除 export 宏，移除所有守卫 |
| `system/inc/emdevif/system/heap.hpp` | 移除 export 宏，移除所有守卫 |
| `system/inc/emdevif/system/event_group.hpp` | 移除 export 宏，移除所有守卫 |
| `system/inc/emdevif/system_impl/thread.inl` | 移除 export 宏 |
| `system/inc/emdevif/system_impl/sys_queue.inl` | 移除 export 宏 |
| `system/inc/emdevif/system_impl/semaphore.inl` | 移除 export 宏（2处） |
| `system/inc/emdevif/system_impl/mutex.inl` | 移除 export 宏 |
| `system/inc/emdevif/system_impl/heap.inl` | 移除 export 宏 |
| `system/inc/emdevif/system_impl/event_group_implements.inl` | 移除 BEGIN/END 块 |
| `system/inc/emdevif/system_impl/event_group_definitions.hpp` | 移除所有守卫 |
| `system/atomic.cppm` | 重构 |
| `system/sys_message_slot.cppm` | 重构 |
| `system/Implements/FreeRTOS/modules/thread.cppm` | 重构 |
| `system/Implements/FreeRTOS/modules/sys_queue.cppm` | 重构 |
| `system/Implements/FreeRTOS/modules/semaphore.cppm` | 重构 |
| `system/Implements/FreeRTOS/modules/mutex.cppm` | 重构 |
| `system/Implements/FreeRTOS/modules/heap.cppm` | 重构 |
| `system/Implements/FreeRTOS/modules/event_group.cppm` | 重构 |
| `system/Implements/FreeRTOS/thread.cpp` | 检查 |
| `system/Implements/FreeRTOS/semaphore.cpp` | 检查 |
| `system/Implements/FreeRTOS/mutex.cpp` | 检查 |
| `system/Implements/FreeRTOS/event_group.cpp` | 检查 |

### emdevif/peripheral

| 文件 | 变更类型 |
|------|---------|
| `peripheral/model/inc/emdevif/peripheral/model/gpio.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/model/inc/emdevif/peripheral/model/can.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/model/inc/emdevif/peripheral/model/serial.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/model/inc/emdevif/peripheral/model/spi.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/model/inc/emdevif/peripheral/model/pwm.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/model/inc/emdevif/peripheral/model/timer.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/peripheral_handle_map/inc/emdevif/peripheral/peripheral_handle_map.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/peripheral_impl_/inc/emdevif/peripheral/gpio.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/peripheral_impl_/inc/emdevif/peripheral/can.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/peripheral_impl_/inc/emdevif/peripheral/serial.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/peripheral_impl_/inc/emdevif/peripheral/spi.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/peripheral_impl_/inc/emdevif/peripheral/pwm.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/peripheral_impl_/inc/emdevif/peripheral/timer.hpp` | 移除 export 宏，移除所有守卫 |
| `peripheral/peripheral_impl_/inc/emdevif/peripheral/detail/peripheral_error_handler.hpp` | 移除所有守卫 |
| `peripheral/model/modules/gpio_model.cppm` | 重构 |
| `peripheral/model/modules/can_model.cppm` | 重构 |
| `peripheral/model/modules/serial_model.cppm` | 重构 |
| `peripheral/model/modules/spi_model.cppm` | 重构 |
| `peripheral/model/modules/pwm_model.cppm` | 重构 |
| `peripheral/model/modules/timer_model.cppm` | 重构 |
| `peripheral/peripheral_handle_map/PeripheralHandleMap.cppm` | 重构 |
| `peripheral/peripheral_impl_/Gpio.cppm` | 重构 |
| `peripheral/peripheral_impl_/Can.cppm` | 重构 |
| `peripheral/peripheral_impl_/Serial.cppm` | 重构 |
| `peripheral/peripheral_impl_/Spi.cppm` | 重构 |
| `peripheral/peripheral_impl_/Pwm.cppm` | 重构 |
| `peripheral/peripheral_impl_/Timer.cppm` | 重构 |

### emdevif_stm32_peripheral

| 文件 | 变更类型 |
|------|---------|
| `inc/emdevif/stm32_peripheral/hal/usart.hpp` | 移除 export 宏，移除所有守卫 |
| `inc/emdevif/stm32_peripheral/hal/spi.hpp` | 移除 export 宏，移除所有守卫 |
| `inc/emdevif/stm32_peripheral/hal/pwm.hpp` | 移除 export 宏（4处），移除所有守卫 |
| `inc/emdevif/stm32_peripheral/hal/gpio.hpp` | 移除 export 宏，移除所有守卫 |
| `inc/emdevif/stm32_peripheral/hal/can.hpp` | 移除 export 宏，移除所有守卫 |
| `inc/emdevif/stm32_peripheral/hal/detail/hal_status_mapper.hpp` | 移除所有守卫 |
| `STM32_HAL_Driver/usart_hal_impl.cppm` | 重构 |
| `STM32_HAL_Driver/spi_hal_impl.cppm` | 重构 |
| `STM32_HAL_Driver/pwm_hal_impl.cppm` | 重构 |
| `STM32_HAL_Driver/gpio_hal_impl.cppm` | 重构 |
| `STM32_HAL_Driver/can_hal_impl.cppm` | 重构 |

### rmdev

| 文件 | 变更类型 |
|------|---------|
| `modules/rmdev_math/inc/rmdev/math.hpp` | 聚合，无变更 |
| `modules/rmdev_math/inc/rmdev/math/basic.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_math/inc/rmdev/math/const_value.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_math/inc/rmdev/math/decimal.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_math/inc/rmdev/math/range.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_math/inc/rmdev/math/trigonometric.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_math/inc/rmdev/matrix.hpp` | 聚合 |
| `modules/rmdev_math/inc/rmdev/matrix/matrix_base.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_math/inc/rmdev/matrix/detail/arm_matrix/ArmMatrixTraits.hpp` | 移除所有守卫 |
| `modules/rmdev_math/inc/rmdev/matrix/detail/arm_matrix/ArmMatrix.hpp` | 移除所有守卫 |
| `modules/rmdev_math/inc/rmdev/matrix/detail/arm_matrix/ArmMatrix.inl` | 移除 export 宏 |
| `modules/rmdev_message_manager/inc/rmdev/message_manager.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_message_manager/inc/rmdev/message_manager/topic.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_message_manager/inc/rmdev/message_manager/subscriber.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_control_algorithm/inc/rmdev/control_algorithm/pid.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_device_model/inc/rmdev/device_model/motor.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_device_model/inc/rmdev/device_model/sensor/imu.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_kinematic_solution/inc/rmdev/kinematic_solution/chassis/OmniWheelInvSolver.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_kinematic_solution/inc/rmdev/kinematic_solution/chassis/FourWheelChassisSolver.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_debug_assistance/inc/rmdev/debug_assistance/vofa.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_ins/inc/rmdev/ins.hpp` | 聚合 |
| `modules/rmdev_ins/inc/rmdev/ins/ins_base.hpp` | 移除 export 宏，移除所有守卫 |
| `modules/rmdev_ins/inc/rmdev/ins/detail/quaternion_ekf_ins/QuaternionEKF_INS.hpp` | 移除 export 宏，移除所有守卫 |
| `drivers/rmdev_driver_DJIMotor/inc/rmdev/driver/dji_motor.hpp` | 移除所有守卫 |
| `drivers/rmdev_driver_DJIMotor/inc/rmdev/driver/dji_motor/can_address.hpp` | 移除 export 宏，移除所有守卫 |
| `drivers/rmdev_driver_DJIMotor/inc/rmdev/driver/dji_motor/class_dji_motor.hpp` | 移除 export 宏 |
| `drivers/rmdev_driver_DJIMotor/inc/rmdev/driver/dji_motor/dji_motor_group.hpp` | 移除 export 宏 |
| `drivers/rmdev_driver_BMI088/inc/rmdev/driver/bmi088.hpp` | 移除 export 宏（2处），移除所有守卫 |
| `modules/rmdev_math/math.cppm` | 重构 |
| `modules/rmdev_math/Matrix.cppm` | 重构 |
| `modules/rmdev_message_manager/message_manager.cppm` | 聚合，可能无结构变更 |
| `modules/rmdev_message_manager/Topic.cppm` | 重构 |
| `modules/rmdev_message_manager/Subscriber.cppm` | 重构 |
| `modules/rmdev_control_algorithm/modules/pid.cppm` | 重构 |
| `modules/rmdev_device_model/motor/Motor.cppm` | 重构 |
| `modules/rmdev_device_model/sensor/Imu.cppm` | 重构 |
| `modules/rmdev_kinematic_solution/chassis/OmniWheelInvSolver.cppm` | 重构 |
| `modules/rmdev_kinematic_solution/chassis/FourWheelChassisSolver.cppm` | 重构 |
| `modules/rmdev_debug_assistance/vofa.cppm` | 重构 |
| `modules/rmdev_ins/ins.cppm` | 重构 |
| `drivers/rmdev_driver_DJIMotor/dji_motor.cppm` | 重构 |
| `drivers/rmdev_driver_BMI088/Bmi088.cppm` | 重构 |
