# 工具模块 (utils)

## 概述

工具模块提供了一组实用工具，用于简化常见的编程任务。该模块包含三个主要组件：

1. **when** - 通用化的 switch 表达式
2. **InitOnce** - 单次赋值包装器
3. **BitInt** - 任意位宽整数

## 设计理念

### 通用化传统控制结构

`when` 表达式是 Kotlin 风格的 `when` 的 C++ 实现，它比传统的 `switch` 更灵活：

- 支持任意类型的比较（不仅仅是整数常量）
- 可以返回值
- 支持默认分支

### 嵌入式环境的特殊需求

- **InitOnce**：用于一次性初始化配置或单例模式
- **BitInt**：支持嵌入式系统中常见的位域操作

## 核心组件详解

### when 表达式

`when` 是一个通用化的 switch 表达式，灵感来自 Kotlin。

```cpp
int result = emdevif::when(value,
    1, [&] { return -1; },
    2, [&] { return -2; },
    emdevif::default_tag, [&] { return 0; }
);
```

**关键特性：**

- **任意类型比较**：支持任何可使用 `==` 比较的类型
- **返回值**：类似于三元运算符，但更清晰
- **默认分支**：使用 `default_tag` 作为最后一个分支

**约束条件：**

1. 参数必须成对：`(case_value, lambda)`
2. 如果使用 `default_tag`，必须是最后一个对
3. 所有 lambda 必须返回相同的类型
4. case 值的比较必须是 `noexcept`

**易混淆点：**

1. **没有匹配时的默认值**：如果没有匹配且没有 `default_tag`，返回 `R()`（默认构造的返回类型）
2. **比较的 noexcept 要求**：case 值的比较必须是 `noexcept`
3. **lambda 返回类型的一致性**：所有 lambda 必须返回相同的类型

### InitOnce 单次赋值包装器

`InitOnce` 是一个包装器，允许值被初始化恰好一次。

```cpp
emdevif::InitOnce<int> config;

// 第一次初始化
auto result = config.init(42);  // 返回 ErrorCode::Success

// 第二次初始化
result = config.init(100);  // 返回 ErrorCode::AlreadyExists

// 访问值
int value = config;  // 42
```

**关键特性：**

- **单次初始化**：只能初始化一次
- **错误码返回**：重复初始化返回 `ErrorCode::AlreadyExists`
- **隐式转换**：可以隐式转换为底层类型
- **默认构造**：默认构造时处于"未初始化"状态

**易混淆点：**

1. **默认构造的值**：默认构造时，值是默认构造的（对于 `int` 是 0），但 `is_init` 是 `false`
2. **读取未初始化的值**：读取未初始化的值不会返回错误，而是返回默认构造的值
3. **拷贝和移动构造**：拷贝和移动构造会将目标标记为已初始化

### BitInt 任意位宽整数

`BitInt` 和 `UBitInt` 提供了任意位宽的整数类型，支持 0 到 64 位。

```cpp
// 12 位无符号整数
emdevif::UBitInt<12> value(4095);  // 最大值

// 8 位有符号整数
emdevif::BitInt<8> signed_value(-128);  // 最小值

// 0 位整数（始终为 0）
emdevif::UBitInt<0> zero;
```

**存储类型选择：**

| 位宽 | 无符号存储类型 | 有符号存储类型 |
|------|--------------|--------------|
| 1-8 | `uint8_t` | `int8_t` |
| 9-16 | `uint16_t` | `int16_t` |
| 17-32 | `uint32_t` | `int32_t` |
| 33-64 | `uint64_t` | `int64_t` |

**运算符行为：**

运算符返回底层类型，不是 `BitInt`。如果需要保持位宽，需要显式构造：

```cpp
emdevif::UBitInt<4> a(15);  // 0b1111
emdevif::UBitInt<4> b(1);   // 0b0001

auto sum = a + b;  // 类型是 uint8_t，值是 16

emdevif::UBitInt<4> wrapped(a + b);  // 值是 0（16 截断到 4 位）
```

**易混淆点：**

1. **运算符返回类型**：运算符返回底层类型，不是 `BitInt`
2. **隐式转换**：`BitInt` 可以隐式转换为任何整数类型
3. **0 位整数**：`UBitInt<0>` 和 `BitInt<0>` 始终为 0

## 最佳实践

### 1. 使用 when 进行清晰的分支

```cpp
// 不好的做法：嵌套的 if-else
if (state == State::Idle) {
    return handleIdle();
} else if (state == State::Running) {
    return handleRunning();
} else if (state == State::Error) {
    return handleError();
} else {
    return handleUnknown();
}

// 好的做法：使用 when
return emdevif::when(state,
    State::Idle, [&] { return handleIdle(); },
    State::Running, [&] { return handleRunning(); },
    State::Error, [&] { return handleError(); },
    emdevif::default_tag, [&] { return handleUnknown(); }
);
```

### 2. 使用 InitOnce 进行安全初始化

```cpp
class Singleton {
private:
    static emdevif::InitOnce<Singleton*> instance_;

public:
    static ErrorCode init() {
        return instance_.init(new Singleton());
    }

    static Singleton* getInstance() {
        return instance_;
    }
};
```

### 3. 使用 BitInt 进行位域操作

```cpp
// 硬件寄存器定义
struct Register {
    emdevif::UBitInt<3> mode;      // 位 0-2
    emdevif::UBitInt<5> address;   // 位 3-7
    emdevif::UBitInt<8> data;      // 位 8-15
};
```

## 易混淆易出错的功能

### 1. when 的默认返回值

```cpp
int value = 10;
int result = emdevif::when(value,
    1, [&] { return 100; },
    2, [&] { return 200; }
    // 没有 default_tag
);
// result 是 0（int 的默认构造值）
```

### 2. InitOnce 的默认构造值

```cpp
emdevif::InitOnce<int> config;

// 读取未初始化的值
int value = config;  // 0（int 的默认构造值）

// 检查是否已初始化
if (config.is_init()) {
    // 不会执行
}
```

### 3. BitInt 的运算符返回类型

```cpp
emdevif::UBitInt<4> a(15);
emdevif::UBitInt<4> b(1);

auto sum = a + b;  // 类型是 uint8_t，值是 16

emdevif::UBitInt<4> wrapped(a + b);  // 值是 0
```

## 容易让用户感到意外的设计

### 1. when 的 noexcept 要求

```cpp
std::string value = "hello";

// 错误！std::string 的比较不是 noexcept
// int result = emdevif::when(value,
//     std::string("hi"), [&] { return 1; },
//     std::string("hello"), [&] { return 2; }
// );

// 正确的做法：使用 const char*
const char* cvalue = "hello";
int result = emdevif::when(cvalue,
    "hi", [&] { return 1; },
    "hello", [&] { return 2; }
);
```

### 2. InitOnce 的拷贝行为

```cpp
emdevif::InitOnce<int> original;
original.init(42);

// 拷贝构造
emdevif::InitOnce<int> copy(original);  // copy.is_init() 是 true

// 移动构造
emdevif::InitOnce<int> moved(std::move(original));  // moved.is_init() 是 true
```

### 3. BitInt 的隐式转换

```cpp
emdevif::UBitInt<4> value(15);

// 隐式转换为任何整数类型
int i = value;          // 15
uint8_t u8 = value;     // 15
uint16_t u16 = value;   // 15
uint32_t u32 = value;   // 15

// 这可能导致意外的类型提升
auto result = value + 1000;  // 类型是 int，值是 1015
```

## 使用示例

### 状态机实现

```cpp
enum class State { Idle, Running, Paused, Error };

class StateMachine {
private:
    State state_ = State::Idle;

public:
    int handleEvent(Event event) {
        return emdevif::when(std::pair{state_, event},
            std::pair{State::Idle, Event::Start}, [&] {
                state_ = State::Running;
                return 0;
            },
            std::pair{State::Running, Event::Pause}, [&] {
                state_ = State::Paused;
                return 0;
            },
            emdevif::default_tag, [&] {
                return -1;  // 无效的状态转换
            }
        );
    }
};
```

## 总结

工具模块为嵌入式系统提供了实用的编程工具：

- **when**：通用化的 switch 表达式，支持任意类型比较
- **InitOnce**：单次赋值包装器，用于安全的一次性初始化
- **BitInt**：任意位宽整数，支持位域操作

理解这些工具的设计理念和使用场景，可以帮助编写出更清晰、更安全的嵌入式代码。