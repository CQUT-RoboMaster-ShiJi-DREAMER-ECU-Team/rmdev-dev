# 类型特征模块 (type_traits)

## 概述

类型特征模块提供了一些扩展的类型特征，用于支持 emdevif 的高级功能。该模块主要包含：

1. **is_consteval** - 检测是否在编译时上下文中

## 设计理念

### 编译时计算的支持

emdevif 大量使用编译时计算（constexpr/consteval），因此需要能够检测当前是否在编译时上下文中。

## 核心组件详解

### is_consteval 检测

`is_consteval` 是一个 `consteval` 函数，用于检测是否在编译时上下文中。

```cpp
consteval bool is_consteval(auto expr) {
    return requires { std::bool_constant<(expr(), false)>{}; };
}
```

**使用示例：**

```cpp
constexpr auto a = 1;
static_assert(emdevif::is_consteval([] { return a; }));  // true

auto b = 0;
static_assert(!emdevif::is_consteval([] { return b; })); // false
```

**实现原理：**

- 使用 requires 表达式检查 `std::bool_constant<(Expr{}(), false)>` 是否是有效类型
- 这只在常量求值中有效

**易混淆点：**

- 表达式必须是可调用的（lambda 或函数对象）
- 表达式不能有副作用（因为在编译时求值）

## 总结

类型特征模块为 emdevif 提供了编译时上下文检测能力：

- **is_consteval**：检测编译时上下文，支持编译时分发

理解这些特征的设计理念和使用场景，可以帮助编写出更灵活、更高效的模板代码。