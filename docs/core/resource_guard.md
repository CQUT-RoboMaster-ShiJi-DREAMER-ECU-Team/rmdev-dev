# 资源保护模块 (resource_guard)

## 概述

资源保护模块提供了一套 RAII（资源获取即初始化）机制，用于确保资源在作用域结束时被正确释放。该模块包含四个组件：

1. **Defer** - 作用域结束时执行清理操作
2. **try-finally** - 类似于其他语言的 try-finally 模式
3. **LockGuard** - 互斥锁的 RAII 包装
4. **InitGuard** - 初始化/反初始化的 RAII 包装

## 设计理念

### 嵌入式环境的资源管理挑战

在嵌入式系统中，资源管理尤为重要：
- 外设资源（GPIO、UART、SPI 等）需要正确初始化和释放
- 内存资源有限，不能依赖垃圾回收
- 异常通常被禁用，需要其他机制确保清理代码执行

### 无异常环境下的安全释放

传统的 C++ 使用异常来确保资源释放（通过析构函数）。但在嵌入式环境中：
- 异常可能被禁用（`-fno-exceptions`）
- 即使启用异常，异常处理也有较大的代码和运行时开销

因此，emdevif 提供了多种 RAII 机制，适应不同的使用场景。

## 核心组件详解

### Defer 守卫

`Defer` 是最简单的 RAII 包装，类似于 Go 语言的 `defer` 语句。

```cpp
{
    // cleanup 通常不会被直接使用，建议与 EMDEVIF_MAYBE_UNUSED 搭配
    EMDEVIF_MAYBE_UNUSED emdevif::Defer cleanup([&]() noexcept {
        releaseResource();
        closeConnection();
    });

    // 使用资源...
}  // cleanup 在这里执行，确保资源被释放
```

**关键特性：**
- 模板参数 `Func` 必须满足 `DeferFunction` 概念：`std::is_nothrow_invocable_r_v<void, Func>`
- 不可复制、不可移动（防止意外的生命周期问题）
- 支持拷贝和移动构造函数

**使用场景：**
- 临时资源的清理
- 多个资源需要按特定顺序释放
- 复杂的清理逻辑

### try-finally 模式

`tryFinally` 提供了类似于其他语言的 try-finally 语义。

```cpp
auto result = emdevif::tryFinally(
    [&] {
        // try 块：执行可能失败的操作
        return performOperation();
    },
    [&]() noexcept {
        // finally 块：无论成功与否都会执行
        cleanupResources();
    }
);
```

**关键特性：**
- finally 函数必须是 `nothrow_invocable_r<void>`
- try 函数可以返回任意类型
- 返回 try 函数的结果

**使用场景：**
- 需要确保清理代码执行，即使操作失败
- 操作有多个可能的退出点

### LockGuard 守卫

`LockGuard` 是互斥锁的 RAII 包装，专为 emdevif 的锁抽象设计。

```cpp
// 假设有一个符合 ValidLock 概念的锁
emdevif::LockGuard guard(mutex);
// 锁已获取

// 临界区操作...

// guard 析构时自动释放锁
```

**ValidLock 概念要求：**
```cpp
template<typename T>
concept ValidLock = requires(T& lock) {
    { lock.lock() } -> std::same_as<ErrorCode>;
    { lock.lock(int{}) } -> std::same_as<ErrorCode>;
    { lock.try_lock() } -> std::same_as<ErrorCode>;
    { lock.unlock() } -> std::same_as<void>;
};
```

**两种构造模式：**

1. **异常模式**（当 `__cpp_exceptions` 启用时）：
   ```cpp
   LockGuard guard(lock);  // 将会自动调用 lock.lock() ，失败抛异常
   ```

2. **非异常模式**：
   ```cpp
   // 使用标签分发，不自动加锁
   LockGuard guard(lock, lock_guard_do_not_lock_when_init); // 仅初始化守卫，不会自动加锁
   ErrorCode ec = guard.lock();  // 手动加锁。如果失败，可以通过 ec 获取错误码
   ```

**注意事项：**
- 不是为 `std::mutex` 设计的（`std::mutex::lock()` 返回 `void`）
- 适用于 emdevif 自己的锁抽象，它们返回 `ErrorCode`

### InitGuard 守卫

`InitGuard` 用于管理具有 `init/deInit` 生命周期的对象，这在嵌入式驱动中很常见。

```cpp
MyDriver driver;
{
    emdevif::InitGuard guard(driver, config);
    // driver.init(config) 已被调用。若失败，抛异常
    
    // 使用驱动...
    
}  // guard 析构时调用 driver.deInit()
```

**ValidHaveInitDeInitPairObject 概念要求：**
```cpp
template<typename T, typename... InitArgs>
concept ValidHaveInitDeInitPairObject = requires(T& obj, InitArgs... args) {
    { obj.init(args...) } -> std::same_as<ErrorCode>;
    { obj.deInit() } -> std::same_as<void>;
};
```

**两种构造模式：**

1. **异常模式**（当 `__cpp_exceptions` 启用时）：
   ```cpp
   InitGuard guard(driver, config);  // 将会自动调用 driver.init(config)，失败抛异常
   ```

2. **非异常模式**：
   ```cpp
   // 使用标签分发，不自动初始化
   InitGuard guard(driver, init_guard_do_not_init_object_tag);  // 仅初始化守卫，不会自动调用 init
   ErrorCode ec = driver.init(config);  // 手动初始化。如果失败，可以通过 ec 获取错误码
   ```

**使用场景：**
- 外设驱动的初始化/反初始化
- 需要确保 `deInit()` 被调用的资源

## 最佳实践

### 1. 选择合适的守卫类型

```cpp
// 简单的清理操作 -> Defer
emdevif::Defer cleanup([&]() noexcept { close(fd); });

// 需要返回值的操作 -> tryFinally
auto result = emdevif::tryFinally(
    [&] { return connect(); },
    [&]() noexcept { disconnect(); }
);

// 互斥锁 -> LockGuard
emdevif::LockGuard guard(mutex);

// 有 init/deInit 的对象 -> InitGuard
emdevif::InitGuard guard(driver, config);
```

### 2. 确保 noexcept 正确使用

```cpp
// 正确：清理函数标记为 noexcept
emdevif::Defer cleanup([&]() noexcept {
    // 不会抛出异常的操作
    releaseResource();
});

// 错误：清理函数可能抛出异常
emdevif::Defer cleanup([&]() {
    throw std::runtime_error("Oops");  // 编译错误或运行时问题
});
```

### 3. 避免生命周期问题

```cpp
// 错误的做法：使用局部变量的引用
void bad() {
    int* ptr = new int(42);
    emdevif::Defer cleanup([&]() noexcept { delete ptr; });
    ptr = nullptr;  // 清理时 ptr 是 nullptr，内存泄漏！
}

// 正确的做法：捕获值或确保引用有效
void good() {
    int* ptr = new int(42);
    auto localPtr = ptr;  // 捕获值
    emdevif::Defer cleanup([localPtr]() noexcept { delete localPtr; });
    ptr = nullptr;  // 没问题，localPtr 仍然有效
}
```

### 4. 处理初始化失败

```cpp
// 非异常模式下的手动初始化
MyDriver driver;
emdevif::InitGuard guard(driver, emdevif::init_guard_do_not_init_object_tag);

auto result = driver.init(config);
if (result != ErrorCode::Success) {
    // 处理错误，guard 析构时不会调用 deInit()
    return result;
}

// 使用驱动...
```

## 易混淆易出错的功能

### 1. Defer 的不可移动性

```cpp
emdevif::Defer a([&]() noexcept { cleanup(); });
// emdevif::Defer b = std::move(a);  // 编译错误！Defer 不可移动
```

这是设计决定，防止意外的生命周期问题。

### 2. tryFinally 的返回值

```cpp
auto result = emdevif::tryFinally(
    [&] { return 42; },
    [&]() noexcept { cleanup(); }
);
// result 是 42，但类型是 int，不是 ErrorCode
```

`tryFinally` 返回 try 块的结果，不检查错误。如果需要错误处理，应该在 try 块中处理。

### 3. LockGuard 与 std::mutex

```cpp
std::mutex mtx;
// emdevif::LockGuard guard(mtx);  // 编译错误！
// std::mutex::lock() 返回 void，不符合 ValidLock 概念
```

需要使用 emdevif 自己的锁抽象，或者使用 `std::lock_guard`。

### 4. InitGuard 的 deInit 调用时机

```cpp
MyDriver driver;
{
    emdevif::InitGuard guard(driver, config);
    // 如果 init() 失败，deInit() 不会被调用
    // 这是正确的，因为对象可能没有完全初始化
}
```

## 容易让用户感到意外的设计

### 1. 异常模式与非异常模式的差异

```cpp
// 异常模式：构造函数可能抛出异常
try {
    emdevif::LockGuard guard(mutex);
    // 临界区
} catch (const emdevif::ErrorWithCodeException& e) {
    // 处理加锁失败
}

// 非异常模式：必须手动检查错误
emdevif::LockGuard guard(mutex, emdevif::lock_guard_do_not_lock_when_init_tag);
auto result = guard.lock();
if (result != ErrorCode::Success) {
    // 处理加锁失败
    return result;
}
```

### 2. 清理函数的执行顺序

```cpp
{
    emdevif::Defer a([&]() noexcept { log("A"); });
    emdevif::Defer b([&]() noexcept { log("B"); });
    // b 先执行，a 后执行（LIFO 顺序）
}
// 输出：B, A
```

### 3. tryFinally 中的异常传播

```cpp
auto result = emdevif::tryFinally(
    [&] {
        throw std::runtime_error("Error");
        return 42;  // 不会执行
    },
    [&]() noexcept {
        cleanup();  // 会执行
    }
);
// 异常会传播到 tryFinally 外部
```

## 使用示例

### 外设驱动管理

```cpp
class SpiDriver {
public:
    ErrorCode init(const SpiConfig& config) {
        // 初始化 SPI 外设
        return ErrorCode::Success;
    }
    
    void deInit() {
        // 释放 SPI 资源
    }
    
    ErrorCode transfer(const uint8_t* tx, uint8_t* rx, size_t len) {
        // 数据传输
        return ErrorCode::Success;
    }
};

void useSpi() {
    SpiDriver spi;
    emdevif::InitGuard guard(spi, {.mode = SpiMode::Mode0, .clock = 1000000});
    
    uint8_t tx[] = {0x01, 0x02, 0x03};
    uint8_t rx[3];
    
    auto result = spi.transfer(tx, rx, sizeof(tx));
    if (result != ErrorCode::Success) {
        // 处理错误
    }
    
    // guard 析构时自动调用 spi.deInit()
}
```

### 临界区保护

```cpp
class CriticalSection {
public:
    ErrorCode lock() {
        __disable_irq();
        return ErrorCode::Success;
    }
    
    ErrorCode lock(int timeout) {
        __disable_irq();
        return ErrorCode::Success;
    }
    
    ErrorCode try_lock() {
        // 尝试进入临界区
        return ErrorCode::Success;
    }
    
    void unlock() {
        __enable_irq();
    }
};

CriticalSection cs;

void protectedOperation() {
    emdevif::LockGuard guard(cs);
    
    // 临界区操作...
    
    // guard 析构时自动调用 cs.unlock()
}
```

### 资源清理链

```cpp
void complexOperation() {
    ResourceA* a = acquireResourceA();
    emdevif::Defer cleanupA([&]() noexcept { releaseResourceA(a); });
    
    ResourceB* b = acquireResourceB(a);
    emdevif::Defer cleanupB([&]() noexcept { releaseResourceB(b); });
    
    ResourceC* c = acquireResourceC(b);
    emdevif::Defer cleanupC([&]() noexcept { releaseResourceC(c); });
    
    // 使用资源...
    
    // 清理顺序：c, b, a（LIFO）
}
```

## 总结

资源保护模块提供了多种 RAII 机制，适应嵌入式环境的不同需求：

- **Defer**：最简单的清理机制，适用于任何清理操作
- **tryFinally**：确保清理代码执行，即使操作失败
- **LockGuard**：互斥锁的安全管理
- **InitGuard**：驱动对象的生命周期管理

理解这些工具的设计理念和使用场景，可以帮助编写出更安全、更可靠的嵌入式代码。