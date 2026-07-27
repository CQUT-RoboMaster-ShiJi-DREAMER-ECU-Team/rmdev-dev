# 编译器属性与实用宏 (attributes_and_useful_macros)

## 概述

编译器属性与实用宏模块提供了跨编译器（GCC、Clang、MSVC）的统一抽象，用于常见的编译器属性和实用宏。这是 emdevif 中最底层的头文件，几乎其他所有头文件都依赖它。

## 设计理念

### 跨编译器兼容性

不同的编译器使用不同的语法来指定属性：

- **GCC/Clang**：`__attribute__((...))`
- **MSVC**：`__declspec(...)` 或 `[[...]]`
- **C++ 标准**：`[[...]]`

emdevif 提供了统一的宏，隐藏了这些差异。

### C/C++ 双语言支持

`.h` 后缀表示该头文件在 C 和 C++ 中都可以使用。属性定义会根据语言模式自动选择合适的语法：

- **C++ 模式**：使用 `[[...]]` 标准语法
- **C 模式**：使用编译器特定的扩展

## 核心属性详解

### 函数属性

| 宏 | 用途 | 等价语法 |
|----|------|----------|
| `EMDEVIF_NO_RETURN` | 标记函数不会返回 | `[[noreturn]]` / `__attribute__((noreturn))` |
| `EMDEVIF_ALWAYS_INLINE` | 强制内联 | `__attribute__((always_inline))` / `__forceinline` |
| `EMDEVIF_USED` | 防止链接器剥离符号 | `__attribute__((used))` |
| `EMDEVIF_WEAK` | 弱符号链接 | `__attribute__((weak))` |
| `EMDEVIF_FALL_THROUGH` | 标记 switch 穿透 | `[[fall_through]]` |
| `EMDEVIF_FORMAT_CHECK(func, si, fc)` | printf 风格格式检查 | `__attribute__((format(...)))` |

### 变量属性

| 宏 | 用途 | 等价语法 |
|----|------|----------|
| `EMDEVIF_MAYBE_UNUSED` | 抑制未使用变量警告 | `[[maybe_unused]]` / `__attribute__((unused))` |

### 链接器段属性

| 宏 | 用途 | 等价语法 |
|----|------|----------|
| `EMDEVIF_DATA_SECTION(name)` | 将数据放入指定段 | `__attribute__((section(name)))` |
| `EMDEVIF_FUNC_SECTION(name)` | 将函数放入指定段 | `__attribute__((section(name)))` |

## 实用宏详解

### EMDEVIF_UNUSED(X)

抑制未使用变量的警告。

```cpp
void callback(int value, void* user_data) {
    EMDEVIF_UNUSED(user_data);  // 抑制未使用警告
    process(value);
}
```

### EMDEVIF_COUNTOF(array)

获取 C 风格数组的元素数量，编译时求值。

```cpp
int arr[10];
constexpr size_t count = EMDEVIF_COUNTOF(arr);  // 10

// 与 sizeof 配合使用
constexpr size_t size = sizeof(arr[0]) * EMDEVIF_COUNTOF(arr);
```

### EMDEVIF_EXTERN_C_BEGIN/END

C/C++ 兼容的 `extern "C"` 包装。

```cpp
// 头文件中
EMDEVIF_EXTERN_C_BEGIN

void c_function(int value);
int c_function_returning(void);

EMDEVIF_EXTERN_C_END
```

## 最佳实践

### 1. 使用 EMDEVIF_NO_RETURN 标记终止函数

```cpp
EMDEVIF_NO_RETURN void terminate() {
    while (true) {}
}

EMDEVIF_NO_RETURN void fatal_error(const char* msg) {
    log_error(msg);
    terminate();
}
```

### 2. 使用 EMDEVIF_FORMAT_CHECK 捕获格式错误

`EMDEVIF_FORMAT_CHECK` 只能放在函数声明的后面，不能放在前面：

```cpp
// 正确：放在函数声明后面
void log_message(const char* format, ...) EMDEVIF_FORMAT_CHECK(printf, 1, 2);

// 错误：放在函数声明前面
EMDEVIF_FORMAT_CHECK(printf, 1, 2)
void log_message(const char* format, ...);
```

使用示例：

```cpp
void log_message(const char* format, ...) EMDEVIF_FORMAT_CHECK(printf, 1, 2);

// 编译时检查
log_message("Value: %d", 42);      // OK
log_message("Value: %d", "hello");  // 编译警告！
```

### 3. 使用 EMDEVIF_USED 保留调试函数

```cpp
EMDEVIF_USED
static void debug_dump(const void* data, size_t size) {
    // 即使在发布版本中也不被剥离
    // 方便调试时调用
}
```

### 4. 使用段属性进行初始化

```cpp
// 将初始化函数放入特定段
EMDEVIF_FUNC_SECTION(".init_array")
static void early_init() {
    // 系统启动时自动调用
}

// 将常量数据放入特定段
EMDEVIF_DATA_SECTION(".rodata.special")
static const uint32_t special_data[] = {1, 2, 3, 4};
```

## 易混淆易出错的功能

### 1. EMDEVIF_NO_RETURN 的使用

```cpp
// 正确：函数永远不会返回
EMDEVIF_NO_RETURN void exit_with_error(int code) {
    exit(code);
}

// 错误：函数可能返回
EMDEVIF_NO_RETURN void might_return(bool flag) {
    if (flag) {
        return;  // 未定义行为！
    }
    while (true) {}
}
```

### 2. EMDEVIF_UNUSED 与 EMDEVIF_MAYBE_UNUSED 的区别

`EMDEVIF_UNUSED` 和 `EMDEVIF_MAYBE_UNUSED` 的实现原理不同：
- `EMDEVIF_UNUSED(X)` 通过 `(void)X` 实现，用于抑制未使用变量的警告
- `EMDEVIF_MAYBE_UNUSED` 通过 `__attribute__((unused))` 或 `[[maybe_unused]]` 实现，用于标记变量或函数可能未使用

**建议优先使用 `EMDEVIF_MAYBE_UNUSED`**，因为它更符合 C++ 标准，且可以在更多场景下使用（如函数参数、结构体成员等）。

```cpp
// EMDEVIF_UNUSED：用于局部变量，通过 (void)X 实现
void function() {
    int unused_var = 42;
    EMDEVIF_UNUSED(unused_var);  // OK
}

// EMDEVIF_MAYBE_UNUSED：用于声明时标记，通过属性实现
EMDEVIF_MAYBE_UNUSED static int global_var = 42;

void process(int value, EMDEVIF_MAYBE_UNUSED void* user_data) {
    // user_data 可能未使用，但不会警告
    process_value(value);
}
```

### 3. EMDEVIF_FORMAT_CHECK 的参数

```cpp
// 参数说明：
// EMDEVIF_FORMAT_CHECK(func, string_index, first_to_check)
//
// func: 使用的格式检查函数（如 printf, scanf 等）
// string_index: 格式字符串参数的位置（从 1 开始）
// first_to_check: 第一个可变参数的位置

EMDEVIF_FORMAT_CHECK(printf, 1, 2)
void my_printf(const char* format, ...);

EMDEVIF_FORMAT_CHECK(scanf, 2, 3)
void my_scanf(void* context, const char* format, ...);
```

## 容易让用户感到意外的设计

### 1. EMDEVIF_WEAK 的链接器行为

```cpp
// 弱符号可以被强符号覆盖
EMDEVIF_WEAK void default_handler() {
    // 默认实现
}

// 用户代码中
void default_handler() {
    // 用户自定义实现，覆盖默认
}
```

### 2. 段属性的平台依赖性

```cpp
// 段名称是平台相关的
// ARM Cortex-M 常用段：
// .text, .data, .bss, .rodata
// .init_array, .fini_array

// x86 常用段：
// .text, .data, .bss
// .ctors, .dtors

EMDEVIF_FUNC_SECTION(".init_array")
static void init_func() {}
```

## 使用示例

### 中断处理函数

```cpp
EMDEVIF_NO_RETURN
void HardFault_Handler() {
    // 记录故障信息
    log_fault_info();

    // 永不停止
    while (true) {
        __WFI();  // 等待中断
    }
}
```

### 调试辅助函数

```cpp
EMDEVIF_USED
EMDEVIF_FORMAT_CHECK(printf, 1, 2)
static void debug_print(const char* format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#else
    EMDEVIF_UNUSED(format);
#endif
}
```

### 内存布局控制

```cpp
// 将关键数据放入快速 RAM
EMDEVIF_DATA_SECTION(".fast_ram")
static uint32_t critical_buffer[256];

// 将初始化代码放入特定段
EMDEVIF_FUNC_SECTION(".init")
void system_init() {
    // 早期初始化代码
}
```

### 跨语言接口

```cpp
// C/C++ 兼容的头文件
#ifndef MY_MODULE_H
#define MY_MODULE_H

#include "emdevif/core/attributes_and_useful_macros.h"

EMDEVIF_EXTERN_C_BEGIN

typedef struct {
    int x, y;
} Point;

void process_point(const Point* p);
Point create_point(int x, int y);

EMDEVIF_EXTERN_C_END

#endif  // MY_MODULE_H
```

## 总结

编译器属性与实用宏模块为 emdevif 提供了跨编译器、跨语言的基础抽象：

- **统一接口**：隐藏编译器差异
- **编译时检查**：捕获格式错误等常见问题
- **链接器控制**：精确控制符号和数据的放置
- **调试支持**：保留调试函数，抑制无用警告

理解这些属性和宏的使用场景，可以帮助编写出更健壮、更可移植的嵌入式代码。