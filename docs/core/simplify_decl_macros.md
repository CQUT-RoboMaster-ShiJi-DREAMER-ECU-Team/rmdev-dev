# 简化声明宏 (simplify_decl_macros)

## 概述

简化声明宏模块提供了一组宏，用于减少删除特殊成员函数时的样板代码。这些宏主要用于 emdevif 内部的资源管理类（如 `Defer`、`LockGuard`、`InitGuard` 等），确保它们具有正确的不可复制/不可移动语义。

## 设计理念

### 减少样板代码

在 C++ 中，要使类不可复制或不可移动，需要显式删除四个特殊成员函数：

```cpp
class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
};
```

使用宏可以简化为一行：

```cpp
class NonCopyable {
public:
    NonCopyable() = default;
    EMDEVIF_DELETE_COPY_AND_MOVE(NonCopyable)
};
```

### 清晰的错误信息

`EMDEVIF_REASON_DELETE` 宏允许在删除函数时提供原因，当用户尝试调用被删除的函数时，编译器会显示更清晰的错误信息。

## 核心宏详解

### EMDEVIF_DELETE_COPY_CONSTRUCTOR(ClassName)

删除拷贝构造函数和拷贝赋值运算符。

```cpp
class NonCopyable {
public:
    EMDEVIF_DELETE_COPY_CONSTRUCTOR(NonCopyable)
};

// 等价于：
// NonCopyable(const NonCopyable&) = delete;
// NonCopyable& operator=(const NonCopyable&) = delete;
```

### EMDEVIF_DELETE_MOVE_CONSTRUCTOR(ClassName)

删除移动构造函数和移动赋值运算符。

```cpp
class NonMovable {
public:
    EMDEVIF_DELETE_MOVE_CONSTRUCTOR(NonMovable)
};

// 等价于：
// NonMovable(NonMovable&&) = delete;
// NonMovable& operator=(NonMovable&&) = delete;
```

### EMDEVIF_DELETE_COPY_AND_MOVE(ClassName)

同时删除拷贝和移动的所有特殊成员函数。

```cpp
class LockGuard {
public:
    EMDEVIF_DELETE_COPY_AND_MOVE(LockGuard)
};

// 等价于：
// LockGuard(const LockGuard&) = delete;
// LockGuard& operator=(const LockGuard&) = delete;
// LockGuard(LockGuard&&) = delete;
// LockGuard& operator=(LockGuard&&) = delete;
```

### EMDEVIF_DECLARE_ZERO_INSTANCE(ClassName)

使类完全不可实例化，删除所有特殊成员函数（包括默认构造函数和析构函数）。

```cpp
class StaticOnly {
public:
    EMDEVIF_DECLARE_ZERO_INSTANCE(StaticOnly)

    static void doSomething();
};

// 等价于：
// StaticOnly() = delete;
// ~StaticOnly() = delete;
// StaticOnly(const StaticOnly&) = delete;
// StaticOnly& operator=(const StaticOnly&) = delete;
// StaticOnly(StaticOnly&&) = delete;
// StaticOnly& operator=(StaticOnly&&) = delete;
```

### EMDEVIF_REASON_DELETE(reason)

删除函数并提供原因，当用户尝试调用时，编译器会显示原因信息。

```cpp
class ExceptionDisabled {
public:
    // 当函数被删除时，提供清晰的错误信息
    operator std::exception_ptr() const = EMDEVIF_REASON_DELETE("Exceptions are disabled");
};

// 等价于：
// operator std::exception_ptr() const = delete("Exceptions are disabled");  // C++26
// operator std::exception_ptr() const = delete;  // C++26 之前
```

## 最佳实践

### 1. 在资源管理类中使用

```cpp
class Defer {
public:
    template<typename Func>
    Defer(Func&& func) : func_(std::forward<Func>(func)) {}

    ~Defer() { func_(); }

    EMDEVIF_DELETE_COPY_AND_MOVE(Defer)

private:
    std::function<void()> func_;
};
```

### 2. 在 RAII 包装器中使用

```cpp
class FileHandle {
public:
    FileHandle(const char* filename) : handle_(fopen(filename, "r")) {}
    ~FileHandle() { if (handle_) fclose(handle_); }

    EMDEVIF_DELETE_COPY_CONSTRUCTOR(FileHandle)
    // 允许移动
    FileHandle(FileHandle&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this != &other) {
            if (handle_) fclose(handle_);
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

private:
    FILE* handle_;
};
```

### 3. 在单例类中使用

```cpp
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }

    EMDEVIF_DELETE_COPY_AND_MOVE(Singleton)

    void doSomething();

private:
    Singleton() = default;
    ~Singleton() = default;
};
```

### 4. 在零例模式（只有静态函数的类）中使用

```cpp
class MathUtils {
public:
    EMDEVIF_DECLARE_ZERO_INSTANCE(MathUtils)

    static int add(int a, int b) { return a + b; }
    static int multiply(int a, int b) { return a * b; }
};
```

## 易混淆易出错的功能

### 1. 宏的使用位置

```cpp
class MyClass {
public:
    // 宏应该放在访问说明符之后（尽量是 public 的）
    EMDEVIF_DELETE_COPY_AND_MOVE(MyClass)
};
```

### 2. 与默认实现的交互

```cpp
class PartiallyCopyable {
public:
    // 删除移动，允许拷贝
    EMDEVIF_DELETE_MOVE_CONSTRUCTOR(PartiallyCopyable)
    // 拷贝构造函数仍然可用
};

PartiallyCopyable a;
PartiallyCopyable b = a;  // OK
PartiallyCopyable c = std::move(a);  // 错误！
```

### 3. EMDEVIF_DECLARE_ZERO_INSTANCE 的效果

```cpp
class StaticOnly {
public:
    EMDEVIF_DECLARE_ZERO_INSTANCE(StaticOnly)
    static void func();
};

StaticOnly::func();  // OK
StaticOnly obj;      // 错误！
StaticOnly* ptr;     // OK，指针可以声明
```

## 容易让用户感到意外的设计

### 1. 宏的参数必须是类名

```cpp
// 正确
class MyClass {
    EMDEVIF_DELETE_COPY_AND_MOVE(MyClass)
};

// 错误：参数必须是类名
class MyClass {
    EMDEVIF_DELETE_COPY_AND_MOVE(this)  // 编译错误
};
```

### 2. 与继承的交互

```cpp
class Base {
public:
    EMDEVIF_DELETE_COPY_AND_MOVE(Base)
};

class Derived : public Base {
    // Derived 也自动不可复制和不可移动
    // 因为基类的拷贝/移动构造函数被删除了
};
```

### 3. 与模板的交互

```cpp
template<typename T>
class Wrapper {
public:
    Wrapper(T value) : value_(value) {}

    EMDEVIF_DELETE_COPY_AND_MOVE(Wrapper)

private:
    T value_;
};

// 如果 T 是可复制的，Wrapper<T> 仍然不可复制
Wrapper<int> w(42);
// Wrapper<int> w2 = w;  // 错误！
```

## 使用示例

### RAII 锁

```cpp
class Mutex {
public:
    void lock() { /* ... */ }
    void unlock() { /* ... */ }
};

class LockGuard {
public:
    LockGuard(Mutex& mutex) : mutex_(mutex) {
        mutex_.lock();
    }

    ~LockGuard() {
        mutex_.unlock();
    }

    EMDEVIF_DELETE_COPY_AND_MOVE(LockGuard)

private:
    Mutex& mutex_;
};
```

### 作用域守卫

```cpp
class ScopeGuard {
public:
    ScopeGuard(std::function<void()> onExit) : onExit_(onExit) {}

    ~ScopeGuard() {
        if (!dismissed_) {
            onExit_();
        }
    }

    void dismiss() { dismissed_ = true; }

    EMDEVIF_DELETE_COPY_AND_MOVE(ScopeGuard)

private:
    std::function<void()> onExit_;
    bool dismissed_ = false;
};
```

### 不可实例化的工具类

```cpp
class ArrayUtils {
public:
    EMDEVIF_DECLARE_ZERO_INSTANCE(ArrayUtils)

    template<typename T, size_t N>
    static constexpr size_t size(const T (&)[N]) {
        return N;
    }

    template<typename T, size_t N>
    static constexpr T* begin(T (&arr)[N]) {
        return arr;
    }

    template<typename T, size_t N>
    static constexpr T* end(T (&arr)[N]) {
        return arr + N;
    }
};
```

### 带有移动语义的类

```cpp
class UniqueResource {
public:
    UniqueResource(int handle) : handle_(handle) {}
    ~UniqueResource() { release(); }

    // 允许移动
    UniqueResource(UniqueResource&& other) noexcept : handle_(other.handle_) {
        other.handle_ = -1;
    }

    UniqueResource& operator=(UniqueResource&& other) noexcept {
        if (this != &other) {
            release();
            handle_ = other.handle_;
            other.handle_ = -1;
        }
        return *this;
    }

    // 删除拷贝
    EMDEVIF_DELETE_COPY_CONSTRUCTOR(UniqueResource)

private:
    int handle_;

    void release() {
        if (handle_ >= 0) {
            // 释放资源
            handle_ = -1;
        }
    }
};
```

## 总结

简化声明宏模块为 C++ 类提供了简洁的方式来控制特殊成员函数：

- **EMDEVIF_DELETE_COPY_CONSTRUCTOR**：删除拷贝语义
- **EMDEVIF_DELETE_MOVE_CONSTRUCTOR**：删除移动语义
- **EMDEVIF_DELETE_COPY_AND_MOVE**：删除所有复制/移动语义
- **EMDEVIF_DECLARE_ZERO_INSTANCE**：使类完全不可实例化
- **EMDEVIF_REASON_DELETE**：提供删除原因

理解这些宏的使用场景和注意事项，可以帮助编写出更安全、更清晰的 C++ 代码。