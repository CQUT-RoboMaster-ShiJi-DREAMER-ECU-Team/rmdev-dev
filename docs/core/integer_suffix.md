# 整型后缀模块 (integer_suffix)

## 概述

整型后缀模块提供了用户定义的字面量（user-defined literals），用于方便地创建固定宽度整数类型。该模块主要用于简化代码，允许使用 `auto` 来声明变量：`auto v = 1_u8`。

## 命名空间

所有字面量都在 `emdevif::literals::integer_literals` 命名空间中，通过内联命名空间组织：

```cpp
using namespace emdevif::literals;  // 引入所有字面量
```

## API 参考

### 无符号整数字面量

| 字面量 | 类型 | 示例 |
|--------|------|------|
| `_u8` | `uint8_t` | `42_u8` |
| `_u16` | `uint16_t` | `1000_u16` |
| `_u32` | `uint32_t` | `100000_u32` |
| `_u64` | `uint64_t` | `10000000000_u64` |

### 有符号整数字面量

| 字面量 | 类型 | 示例 |
|--------|------|------|
| `_i8` | `int8_t` | `-42_i8` |
| `_i16` | `int16_t` | `-1000_i16` |
| `_i32` | `int32_t` | `-100000_i32` |
| `_i64` | `int64_t` | `-10000000000_i64` |

### 特殊字面量

| 字面量 | 类型 | 示例 | 说明 |
|--------|------|------|------|
| `_ptrdiff` | `std::ptrdiff_t` | `42_ptrdiff` | 指针差值类型 |
| `_zu` | `std::size_t` | `42_zu` | 大小类型（`_z` 是 C++ 标准保留的） |

## 使用示例

```cpp
using namespace emdevif::literals;

// 无符号整数
auto small = 42_u8;          // uint8_t
auto medium = 1000_u16;      // uint16_t
auto large = 100000_u32;     // uint32_t
auto huge = 10000000000_u64; // uint64_t

// 有符号整数
auto neg_small = -42_i8;     // int8_t
auto neg_medium = -1000_i16; // int16_t

// 特殊类型
auto size = 42_zu;           // std::size_t
auto offset = 10_ptrdiff;   // std::ptrdiff_t
```
