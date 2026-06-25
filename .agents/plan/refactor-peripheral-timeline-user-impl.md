# 重构计划：emdevif_peripheral / emdevif_timeline 摆脱 emdevif_user_declares 依赖

## 目标

让 `emdevif_peripheral` 与 `emdevif_timeline` 不再依赖 `emdevif_user_declares` 目标，改为「链接期注入」：

- 库在命名空间 `emdevif::user_impl` 中**声明**（仅声明）所需函数，声明折叠进现有头文件。
- 用户在自己的**源文件**（普通 `.cpp`）中于 `emdevif::user_impl` 命名空间**定义**这些函数，自行编译链接。
- 库的调用处通过 `extern` 函数调用 → 链接器解析符号。**不再要求 constexpr**，定义与调用可跨翻译单元。
- 库**不再链接** `emdevif_user_declares` 目标。

## 现状与问题

当前 `emdevif_user_declares` 是一个用户必须创建的同名 CMake 目标，承担依赖注入缝作用：

| 模块 | 调用方式 | 依赖形式 | constexpr 要求 |
|---|---|---|---|
| timeline | `user_declares::timeline::getMicroseconds()` | 头文件 `#include "emdevif/user_declares.hpp"` 或 `import emdevif.user_declares`，由 `include_emdevif_user_declares_in_headers.hpp` 统一调度 | 函数定义需与调用处同 TU（头文件注入，函数体必须可见） |
| peripheral | `user_declares::peripheral_handle_map`（constexpr `makeStaticMap`） | 同上 + `PUBLIC` 链接 `emdevif_user_declares` | 必须 constexpr（`is_consteval` static_assert 校验），`.at()` 编译期查表 |

关键文件：

- `timeline/inc/emdevif/timeline.hpp:10,51,66` — include 辅助头 + 调用 `user_declares::timeline::*`
- `timeline/timeline.cppm:8-10` — GMF 中条件 include user_declares
- `peripheral/peripheral_handle_map/inc/emdevif/peripheral/peripheral_handle_map.hpp:11,23-28,43-52` — include 辅助头 + constexpr 校验 + `findHandle` 委托
- `peripheral/peripheral_handle_map/CMakeLists.txt:9,22` — `PUBLIC emdevif_user_declares`
- `timeline/CMakeLists.txt:17,24` — 链接 `emdevif_user_declares`
- `core/inc/emdevif/core/detail/include_emdevif_user_declares_in_headers.hpp` — 调度辅助头（logger 仍用，保留）

`peripheral_handle_map.at()` 返回 `void**`（指向存储槽），`*handle` 解引用得 `void*`（= Instance 地址），最终 `findHandle` 返回 `void*` 由 impl 层 `static_cast<XxxModel::Instance*>`。

## 目标契约（签名）

```cpp
// timeline（由 EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS 二选一）
namespace emdevif::user_impl::timeline {
    BaseTimePoint getMicroseconds() noexcept;   // 宏为 false 时
    BaseTimePoint getMilliseconds() noexcept;   // 宏为 true 时
}

// peripheral
namespace emdevif::user_impl::peripheral_handle_map {
    void* findHandle(std::string_view name) noexcept;  // 返回 Instance 地址，未找到返回 nullptr
}
```

## 已确认的设计取舍

1. **Peripheral 句柄查找**：改为用户提供 `findHandle(string_view name) -> void*` 函数，库不再关心用户内部数据结构，彻底去掉 constexpr / array_map 依赖。
2. **Timeline 精度宏**：`EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS` 提升为 cache 变量 + `emdevif_timeline` 目标的 **PUBLIC** compile definition，自动传播到库自身 TU 与所有消费者，消除 ODR/链接不一致风险。
3. **user_impl 声明位置**：折叠进现有头文件（`timeline.hpp` / `peripheral_handle_map.hpp`），不新增独立声明头。
4. **Logger 处理范围**：仅重构 peripheral + timeline；logger 保持现状仍用 `emdevif_user_declares`，`include_emdevif_user_declares_in_headers.hpp` 辅助头为 logger 保留。

## 详细改动

### A. timeline 模块

#### A1. `timeline/inc/emdevif/timeline.hpp`

- 删除第 10 行 `#include "emdevif/core/detail/include_emdevif_user_declares_in_headers.hpp"`。
- 在 `using BaseTimePoint = uint64_t;`（第 24 行）之后、`class Timeline` 之前，新增 user_impl 声明块（宏二选一）：

  ```cpp
  namespace user_impl::timeline {
  #if (!EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS)
      BaseTimePoint getMicroseconds() noexcept;
      BaseTimePoint getMilliseconds() noexcept = EMDEVIF_REASON_DELETE("Reason...");
  #else
      BaseTimePoint getMicroseconds() noexcept = EMDEVIF_REASON_DELETE("Reason...");
      BaseTimePoint getMilliseconds() noexcept;
  #endif
  }  // namespace user_impl::timeline
  ```

- 将第 51、66 行 `::emdevif::user_declares::timeline::*` 改为 `::emdevif::user_impl::timeline::*`。
- 移除第 17-19 行 `#ifndef ... #define EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS false` 兜底，改为由目标 PUBLIC 编译定义单一真源传播（避免掩盖「未链接目标」错误）。

#### A2. `timeline/timeline.cppm`

- 删除第 8-10 行 `#ifndef EMDEVIF_USER_DECLARES_PROVIDE_MODULE / #include "emdevif/user_declares.hpp"` 整块（不再需要；`timeline.hpp` 已自带声明）。
- 其余保持（`#include "emdevif/timeline.hpp"` 已带入 user_impl 声明）。

#### A3. `timeline/CMakeLists.txt`

- Modules 分支与 header 分支均删除 `emdevif_user_declares` 链接。
- 新增 cache 变量并以 PUBLIC compile definition 挂到目标：

  ```cmake
  set(EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS OFF CACHE BOOL
      "Timeline 时间源为毫秒精度（ON）或微秒精度（OFF）")
  target_compile_definitions(${PROJECT_NAME} PUBLIC
      EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS=$<BOOL:${EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS}>)
  ```

### B. peripheral 模块

#### B1. `peripheral/peripheral_handle_map/inc/emdevif/peripheral/peripheral_handle_map.hpp`

- 删除第 11 行 `#include "emdevif/core/detail/include_emdevif_user_declares_in_headers.hpp"`。
- 删除第 13-14 行 `#include "emdevif/core/type_traits.hpp"`、`#include "emdevif/core/data_container/array_map.hpp"`（不再需要 `is_consteval` 与 `makeStaticMap`）。
- 删除第 23-28 行两条 `static_assert`（constexpr 校验与类型校验）。
- 在 `namespace emdevif {` 后新增声明：

  ```cpp
  namespace user_impl::peripheral_handle_map {
      void* findHandle(std::string_view name) noexcept;
  }
  ```

- `PeripheralHandleMap::findHandle`（第 43-52 行）改为非 constexpr 委托：

  ```cpp
  static void* findHandle(const std::string_view name) noexcept
  {
      return user_impl::peripheral_handle_map::findHandle(name);
  }
  ```

  （去掉 `constexpr`，去掉内部 `.at()` / `*handle` 逻辑——这些现由用户实现内部完成。）

#### B2. `peripheral/peripheral_handle_map/PeripheralHandleMap.cppm`

- 无需改动（仅 include 头文件 + export using），头文件改动自动生效。

#### B3. `peripheral/peripheral_handle_map/CMakeLists.txt`

- Modules 分支第 9 行、header 分支第 22 行删除 `emdevif_user_declares`。
- 保留 `PUBLIC emdevif_core`（`simplify_declmacros.hpp` 来自 core，仍需链接）。

#### B4. peripheral_impl_ 层（去 constexpr）

各 impl 头文件构造函数 `explicit constexpr Xxx(...)` 需去掉 `constexpr`（因 `findHandle` 现为非 constexpr extern 调用）。涉及文件：

- `peripheral_impl_/inc/emdevif/peripheral/{gpio,pwm,serial,spi,timer,can}.hpp` 中调用 `PeripheralHandleMap::findHandle` 的构造函数。
- 例如 `gpio.hpp:35` `explicit constexpr Gpio(...)` → `explicit Gpio(...)`。

逐一检查 6 个 impl 头文件。

### C. core 辅助头

- `core/inc/emdevif/core/detail/include_emdevif_user_declares_in_headers.hpp` **保留不动**（logger 仍依赖）。timeline/peripheral 不再 include 它。

### D. 文档更新（emdevif 子模块内）

按多仓库协作流程，库文档改动属子模块职责：

- `timeline/README.md`：将「emdevif_user_declares 的需求」改为「user_impl 的需求」，给出用户 `.cpp` 实现示例（不再 `export namespace user_declares`，改为普通 `namespace user_impl`）；删除「不可链接/导入 emdevif_timeline」循环依赖警告（已不适用）。
- `peripheral/README.md` 与 `peripheral/AGENTS.md`：将「在 emdevif_user_declares 中定义 `constexpr auto peripheral_handle_map`」改为「在 `emdevif::user_impl::peripheral_handle_map` 中实现 `findHandle`」；更新数据流示例；删除循环依赖注意事项中关于 user_declares 的条目（保留 model 层可链接的说明）。
- `emdevif/README.md` 第 198-367 行「emdevif_user_declares」章节：补充说明 timeline/peripheral 已改为 user_impl 链接期注入（logger 仍用旧机制），或拆分为两节说明。

## 用户侧迁移示例

### timeline 用户（新写一个 `.cpp`）

```cpp
#include "emdevif/timeline.hpp"  // 带入 user_impl::timeline 声明
#include <cstdint>

namespace emdevif::user_impl::timeline {
uint64_t getMicroseconds() noexcept {
    return /* 读硬件计时器 */;
}
}
```

### peripheral 用户（新写一个 `.cpp`）

```cpp
#include "emdevif/peripheral/peripheral_handle_map.hpp"
#include "emdevif/peripheral/model/gpio.hpp"
#include <string_view>

constinit emdevif::GpioModel::Instance led{ .handle=&hw_led, .write_function=... };

namespace emdevif::user_impl::peripheral_handle_map {
void* findHandle(std::string_view name) noexcept {
    if (name == "led") return &::led;
    return nullptr;  // 当然，用户侧也可以自行使用 emdevif::arrayMap/ststicMap/when 实现映射
}
}
```

用户不再创建 `emdevif_user_declares` 目标，也不再写 `emdevif/user_declares.hpp` 或 `emdevif.user_declares` 模块；只需把自己的 `.cpp` 编进可执行文件即可。

## 测试

当前 host 测试仅启用 `emdevif_core`（`EMDEVIF_ENABLED_MODULES=""`），timeline/peripheral 尚无测试。建议新增链接期注入的回归测试：

- 在 `tests/mock/emdevif/` 下新增 `timeline/` 与 `peripheral/` 测试目录。
- 提供测试用 `user_impl` 实现 `.cpp`（如 `timeline` 用计数器伪造时间；`peripheral` 用伪造 `Instance` 表）。
- 测试目标链接 `emdevif_timeline` / `emdevif_peripheral` + 伪造实现 `.cpp`。
- 验证 Modules ON/OFF 两条 preset 路径（`HostTestModulesExceptions` 与 `HostTestHeadersNoexceptions` 至少各一）。

> 注：宿主测试目录在 rmdev-dev 仓库，需在 `tests/mock/CMakeLists.txt` 注册新测试源并链接相应 emdevif 模块目标（需在测试配置中启用 timeline/peripheral 模块）。

## 验证步骤

1. 在 rmdev-dev 跑通现有 4 个 preset（确保 core/logger 等未受影响）：

   ```
   cmake --preset HostTestModulesExceptions && cmake --build --preset HostTestModulesExceptions --config Debug
   ctest --preset HostTestModulesExceptions -C Debug
   ```

   （Headers/Noexceptions 同理）

2. 新增的 timeline/peripheral 测试在 Modules ON + OFF 两条路径均通过。
3. 人工构造最小用户工程，分别用 `.cpp` 实现 `user_impl`，链接 timeline/peripheral，确认符号在链接期正确解析（含宏二选一两条路径）。
4. 确认 `emdevif_user_declares` 字样在 timeline/peripheral 范围内已完全移除（grep 验证：仅 logger 残留）。

## 范围与注意事项

- **logger 不在本次范围**：仍使用 `emdevif_user_declares` 与辅助头，行为不变。
- **改动归属**：库逻辑/文档改动（A、B、D 节）应在 `emdevif` 子模块仓库提交；集成测试（测试节）在 rmdev-dev 提交。遵循各子模块 AGENTS.md 与编码规范。
- **modules 双路径**：声明仅为纯函数声明，无模块专属语法，GMF 内 include 与普通 include 行为一致，天然满足 header/module 双路径。`user_impl` 函数建议用户以**普通 `.cpp`**（非 module 单元）实现，避免具名模块符号附着问题。
- **破坏性变更**：移除 `constexpr`、命名空间由 `user_declares` 改为 `user_impl`、移除 `peripheral_handle_map` 变量契约——对现有用户属破坏性变更，需在 README/CHANGELOG 标注并提供迁移指引。
