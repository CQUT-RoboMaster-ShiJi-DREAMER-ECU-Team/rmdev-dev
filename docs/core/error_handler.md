# 错误处理模块 (error_handler)

## 概述

错误处理模块是 emdevif 核心架构中最重要的组件，它为嵌入式环境提供了一套完整的错误处理机制。该模块包含三个层次：

1. **错误码类型** (`error_code.hpp`) - 统一的错误码定义
2. **致命错误处理** (`fatal_handler.hpp`) - C++ 侧的核心实现
3. **宏接口** (`fatal_handler.h`) - C/C++ 共享的用户接口

## 设计理念

### 嵌入式环境的错误处理哲学

在嵌入式系统中，异常处理通常被禁用（因为代码大小和运行时开销），因此 emdevif 采用了**错误码返回**而非异常抛出的设计。所有可能失败的操作都返回 `ErrorCode` 类型，调用者必须显式处理错误。

### 分层错误处理

错误被分为三个层次：

1. **可恢复错误** - 使用 `ErrorCode` 返回，调用者可以处理
2. **致命错误** - 使用 `EMDEVIF_FATAL_HANDLER` 触发，程序无法继续执行
3. **断言检查** - 使用 `EMDEVIF_ASSERT`，仅在调试模式下有效

### 零开销抽象

- 错误码是简单的枚举值，没有运行时开销
- 断言在发布版本中被完全移除（通过 `NDEBUG` 宏）
- 致命错误处理函数是可配置的，但默认实现是简单的无限循环

## 核心组件详解

### ErrorCode 类型

`ErrorCode` 是一个轻量级的错误码包装类，提供了以下特性：

```cpp
// 预定义的错误码
enum class ErrorCodeValue : int_fast8_t {
    Success = 0,
    UnknownError = -1,
    InvalidArgument = -2,
    OutOfMemory = -3,
    NotFound = -4,
    PermissionDenied = -5,
    Timeout = -6,
    Full = -7,
    Empty = -8,
    OperationFail = -9,
    AlreadyExists = -10,
    NotImplemented = -11,
    InternalError = -12
};
```

**关键设计决策：**

- **隐式转换**：允许从 `ErrorCodeValue` 隐式转换到 `ErrorCode`，使得 `ErrorCode ec = someFunc()` 的写法自然流畅
- **显式构造**：从其他整型构造需要显式转换，防止意外的类型混用
- **终止检查**：`terminateIfNotSucceed()` 方法在错误时立即终止程序，适用于初始化等关键路径

### 致命错误处理

#### terminate 函数

```cpp
[[noreturn]] void terminate();
```

- 默认实现是无限循环：`while(true) {}`
- 可以通过 `registerTerminateFunction()` 注册自定义实现（如关闭中断后死循环）
- **重要**：注册的函数绝对不能返回，否则是未定义行为

#### fatalHandler 函数

```cpp
[[noreturn]] void fatalHandler(const char* file, int line, const char* format, ...);
```

- 先调用用户注册的致命错误回调（如果有）
- 然后调用 `terminate()`
- 回调函数可以返回（用于日志记录等），但最终程序会终止

#### EMDEVIF_ASSERT 宏

```cpp
EMDEVIF_ASSERT(condition, message);
```

**constexpr 断言的巧妙实现：**

```cpp
(void)(condition || (detail::assertFailedHandler(...) , false) || (terminate(), false));
```

这个实现利用了短路求值：
- 如果 `condition` 为 `true`，后续部分不会执行
- 如果 `condition` 为 `false`，会调用 `assertFailedHandler` 和 `terminate`
- 在编译时求值时，如果 `condition` 为 `false`，访问非 constexpr 的 `assertFailedHandler` 会导致编译错误——这正是我们想要的断言失败效果

**易混淆点**：当编译器报错 "detail::assertFailedHandler was not declared constexpr" 时，这表示断言在编译时失败了，而不是框架的 bug。

### 注册函数的线程安全性

以下函数**不是线程安全的**，只能在初始化阶段调用：

- `registerTerminateFunction()`
- `registerFatalHandler()`
- `registerAssertFailedHandler()`

在多线程环境中，必须在启动任何任务之前完成注册。

## 最佳实践

### 1. 错误处理策略

```cpp
// 好的做法：显式检查错误
ErrorCode result = someOperation();
if (result != ErrorCode::Success) {
    // 处理错误
    return result;
}

// 或者使用 terminateIfNotSucceed() 用于关键初始化
someCriticalInit().terminateIfNotSucceed();
```

### 2. 自定义错误处理

```cpp
// 初始化阶段注册自定义处理函数
emdevif::registerFatalHandler([](const char* file, int line, const char* msg) {
    // 记录错误信息到日志系统
    logError("Fatal at %s:%d: %s", file, line, msg);
});

emdevif::registerTerminateFunction([]() {
    // 安全关闭所有外设
    disableAllPeripherals();
    // 进入低功耗模式或重启
    NVIC_SystemReset();
});
```

### 3. 断言的使用

```cpp
// 调试模式下的参数检查
void processBuffer(uint8_t* buffer, size_t size) {
    EMDEVIF_ASSERT(buffer != nullptr, "Buffer pointer cannot be null");
    EMDEVIF_ASSERT(size > 0 && size <= MAX_BUFFER_SIZE, "Invalid buffer size");
    // ...
}

// 编译时断言（通过 constexpr 上下文）
constexpr auto config = calculateConfig();
EMDEVIF_ASSERT(config.isValid(), "Invalid configuration");  // 编译时检查
```

## 易混淆易出错的功能

### 1. ErrorCode 的隐式转换

```cpp
ErrorCode ec = ErrorCodeValue::Success;  // OK，隐式转换
ErrorCode ec2 = ErrorCode::Success;  // 也 OK，而且更推荐这样做
ErrorCode ec2 = 0;  // 错误！需要显式转换
ErrorCode ec3 = static_cast<ErrorCode>(0);  // OK
```

### 2. 断言的编译时行为

```cpp
constexpr int divide(int a, int b) {
    EMDEVIF_ASSERT(b != 0, "Division by zero");
    return a / b;
}

// 编译时调用：如果 b=0，会导致编译错误
constexpr auto result = divide(10, 2);  // OK
// constexpr auto bad = divide(10, 0);  // 编译错误！
```

## 容易让用户感到意外的设计

### 1. 编译时断言的错误信息

当编译时断言失败时，编译器会报类似这样的错误：

```
error: 'detail::assertFailedHandler' is not a constant expression
```

这看起来像是框架的 bug，但实际上是因为断言条件在编译时为 `false`，导致尝试调用非 constexpr 的 `assertFailedHandler`。

### 2. NDEBUG 对断言的影响

```cpp
#define NDEBUG  // 禁用断言
EMDEVIF_ASSERT(shouldNeverHappen(), "This won't be checked!");
// 断言被完全移除，条件表达式 shouldNeverHappen() 不会被求值
```

### 3. terminate 函数的无限循环

默认的 `terminate()` 实现是 `while(true) {}`，这会导致：
- 看门狗超时复位（如果启用了看门狗）
- 调试器暂停执行
- 在没有看门狗的系统中，程序会永远挂起

## 模块使用

### 导入方式

```cpp
// C++ 模块方式
import emdevif.core.error_handler;

// 头文件方式
#include "emdevif/core/error_handler.hpp"
#include "emdevif/core/fatal_handler.h"  // 用于宏
```

### C 语言使用

```c
// 只需要头文件
#include "emdevif/core/fatal_handler.h"

void example() {
    EMDEVIF_FATAL_HANDLER("Something went wrong: %d", errorCode);
    EMDEVIF_ASSERT(ptr != nullptr, "Null pointer");
}
```

## 总结

错误处理模块的设计体现了嵌入式系统的核心约束：
- **确定性**：没有异常传播，错误处理路径清晰
- **零开销**：错误码是简单的值类型，没有运行时开销
- **可配置**：致命错误处理可以根据平台定制
- **调试友好**：断言提供详细的错误位置信息

理解这些设计理念和潜在陷阱，可以帮助开发者正确使用错误处理机制，编写出健壮的嵌入式代码。