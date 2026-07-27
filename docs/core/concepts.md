# 概念库 (concepts)

## 概述

概念库定义了 emdevif 中使用的各种 C++20 概念，用于约束模板参数，提供更清晰的错误信息和更好的代码可读性。

## 设计理念

### 概念驱动的接口设计

emdevif 广泛使用概念来定义模板参数的约束，这带来了以下好处：

1. **清晰的错误信息**：当模板参数不满足约束时，编译器会给出明确的错误信息
2. **自文档化**：概念名称本身就说明了参数的要求
3. **更好的重载决议**：可以根据概念进行更精确的重载

### 嵌入式环境的特殊考虑

在嵌入式环境中，概念的设计需要考虑：

- **零开销**：概念本身没有运行时开销
- **编译时检查**：所有约束检查都在编译时完成
- **兼容性**：需要与 C++20 模块系统兼容

## 核心概念详解

### HaveLimitType 概念

`HaveLimitType` 是最基础的概念，用于检查类型是否具有最大值和最小值。

```cpp
template<typename T>
concept HaveLimitType = requires {
    // 方式1：通过 std::numeric_limits
    { std::numeric_limits<T>::max() } -> std::same_as<T>;
    { std::numeric_limits<T>::min() } -> std::same_as<T>;
} || requires {
    // 方式2：通过静态成员
    { T::max() } -> std::same_as<T>;
    { T::min() } -> std::same_as<T>;
} || requires(T a) {
    // 方式3：通过非静态成员
    { a.max() } -> std::same_as<T>;
    { a.min() } -> std::same_as<T>;
};
```

**关键特性：**

- 支持三种不同的接口：`std::numeric_limits`、静态成员、非静态成员
- 这是 `ArithmeticType` 的基础

**使用场景：**

```cpp
template<HaveLimitType T>
T clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
```

### ArithmeticType 概念

`ArithmeticType` 是 emdevif 中最重要的概念之一，用于约束算术类型。

```cpp
template<typename Type>
concept ArithmeticType = HaveLimitType<Type> && requires(Type lhs, Type rhs) {
    lhs + rhs;
    lhs - rhs;
    lhs * rhs;
    lhs / rhs;
    lhs = rhs;
    lhs <=> rhs;
};
```

**与 `std::is_arithmetic` 的区别：**

- `std::is_arithmetic` 只接受内置算术类型（`int`、`float` 等）
- `ArithmeticType` 还接受用户定义的类型，只要它们支持算术运算

**示例：**

```cpp
// 内置类型
static_assert(emdevif::ArithmeticType<int>);
static_assert(emdevif::ArithmeticType<float>);

// 用户定义的类型
struct Vector2D {
    float x, y;
    
    Vector2D operator+(const Vector2D& other) const { return {x + other.x, y + other.y}; }
    Vector2D operator-(const Vector2D& other) const { return {x - other.x, y - other.y}; }
    Vector2D operator*(float scalar) const { return {x * scalar, y * scalar}; }
    Vector2D operator/(float scalar) const { return {x / scalar, y / scalar}; }
    
    Vector2D& operator=(const Vector2D& other) { x = other.x; y = other.y; return *this; }
    
    auto operator<=>(const Vector2D&) const = default;
    
    static Vector2D max() { return {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()}; }
    static Vector2D min() { return {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()}; }
};

static_assert(emdevif::ArithmeticType<Vector2D>);  // OK！
```

**易混淆点：**

- `ArithmeticType` 不要求类型支持 `%`（取模）运算
- `operator<=>` 必须返回 `std::strong_ordering`，不能是 `std::weak_ordering` 或 `std::partial_ordering`

### FloatingPointType 概念

`FloatingPointType` 用于约束浮点类型。

```cpp
template<typename T>
concept FloatingPointType = std::is_floating_point_v<T>;
```

**使用场景：**

```cpp
template<FloatingPointType T>
T lerp(T a, T b, T t) {
    return a + t * (b - a);
}
```

### PointerType 概念

`PointerType` 用于约束指针类型。

```cpp
template<typename T>
concept PointerType = std::is_pointer_v<T>;
```

**使用场景：**

```cpp
template<PointerType T>
void terminateIfNullptr(T ptr) {
    if (ptr == nullptr) {
        terminate();
    }
}
```

## 最佳实践

### 1. 使用概念约束模板参数

```cpp
// 好的做法：使用概念约束
template<ArithmeticType T>
T add(T a, T b) {
    return a + b;
}

// 不好的做法：没有约束
template<typename T>
T add(T a, T b) {
    return a + b;  // 如果 T 不支持 +，错误信息不明确
}
```

### 2. 选择合适的概念

```cpp
// 如果只需要算术运算
template<ArithmeticType T>
T calculate(T a, T b) { /* ... */ }

// 如果需要浮点数
template<FloatingPointType T>
T preciseCalculate(T a, T b) { /* ... */ }

// 如果需要指针
template<PointerType T>
void process(T ptr) { /* ... */ }
```

### 3. 自定义概念

```cpp
// 定义自己的概念
template<typename T>
concept Serializable = requires(T& obj, std::vector<uint8_t>& buffer) {
    { obj.serialize(buffer) } -> std::same_as<ErrorCode>;
    { T::deserialize(buffer) } -> std::same_as<std::expected<T, ErrorCode>>;
};

// 使用自定义概念
template<Serializable T>
ErrorCode save(const T& obj) {
    std::vector<uint8_t> buffer;
    return obj.serialize(buffer);
}
```

## 易混淆易出错的功能

### 1. ArithmeticType 与 std::is_arithmetic 的区别

```cpp
// std::is_arithmetic 只接受内置类型
static_assert(std::is_arithmetic_v<int>);      // OK
static_assert(std::is_arithmetic_v<float>);    // OK
static_assert(!std::is_arithmetic_v<Vector2D>); // 用户定义类型不满足

// ArithmeticType 更广泛
static_assert(emdevif::ArithmeticType<int>);      // OK
static_assert(emdevif::ArithmeticType<float>);    // OK
static_assert(emdevif::ArithmeticType<Vector2D>); // OK，如果支持算术运算
```

### 2. HaveLimitType 的三种接口

```cpp
struct StaticLimits {
    static int max() { return 100; }
    static int min() { return 0; }
};

struct MemberLimits {
    int max() const { return 100; }
    int min() const { return 0; }
};

static_assert(emdevif::HaveLimitType<int>);           // 通过 std::numeric_limits
static_assert(emdevif::HaveLimitType<StaticLimits>);   // 通过静态成员
static_assert(emdevif::HaveLimitType<MemberLimits>);   // 通过非静态成员
```

### 3. operator<=> 的要求

```cpp
struct Bad {
    auto operator<=>(const Bad&) const = std::weak_ordering::equivalent;  // 错误！
};

struct Good {
    auto operator<=>(const Good&) const = std::strong_ordering::equivalent;  // OK
};

static_assert(!emdevif::ArithmeticType<Bad>);
static_assert(emdevif::ArithmeticType<Good>);
```

## 容易让用户感到意外的设计

### 1. ArithmeticType 不要求取模运算

```cpp
struct NoModulo {
    int value;
    
    NoModulo operator+(const NoModulo& other) const { return {value + other.value}; }
    NoModulo operator-(const NoModulo& other) const { return {value - other.value}; }
    NoModulo operator*(const NoModulo& other) const { return {value * other.value}; }
    NoModulo operator/(const NoModulo& other) const { return {value / other.value}; }
    
    NoModulo& operator=(const NoModulo& other) { value = other.value; return *this; }
    
    auto operator<=>(const NoModulo&) const = default;
    
    static NoModulo max() { return {100}; }
    static NoModulo min() { return {0}; }
};

static_assert(emdevif::ArithmeticType<NoModulo>);  // OK，即使没有 operator%
```

### 2. HaveLimitType 的优先级

```cpp
struct Ambiguous {
    static int max() { return 100; }
    static int min() { return 0; }
    int max() const { return 200; }
    int min() const { return -100; }
};

// 编译器会选择静态成员还是非静态成员？
// 实际上，这会导致编译错误，因为两个 requires 子句都满足
```

### 3. 概念的子sumption 关系

```cpp
// ArithmeticType 包含 HaveLimitType
template<ArithmeticType T>
void process(T value) { /* ... */ }

// 这意味着 ArithmeticType 的约束比 HaveLimitType 更强
static_assert(emdevif::ArithmeticType<int>);
static_assert(emdevif::HaveLimitType<int>);
// 但反过来不一定成立
```

## 使用示例

### 数学库

```cpp
template<ArithmeticType T>
class Vector2 {
public:
    T x, y;
    
    Vector2 operator+(const Vector2& other) const {
        return {x + other.x, y + other.y};
    }
    
    Vector2 operator*(T scalar) const {
        return {x * scalar, y * scalar};
    }
    
    T length() const requires FloatingPointType<T> {
        return std::sqrt(x * x + y * y);
    }
};

// 使用
Vector2<int> vi = {1, 2};      // OK，int 满足 ArithmeticType
Vector2<float> vf = {1.0f, 2.0f};  // OK，float 满足 ArithmeticType
```

### 算法库

```cpp
template<ArithmeticType T>
T clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

template<FloatingPointType T>
T lerp(T a, T b, T t) {
    return a + t * (b - a);
}

template<PointerType T>
void safeDelete(T& ptr) {
    if (ptr != nullptr) {
        delete ptr;
        ptr = nullptr;
    }
}
```

### 容器库

```cpp
template<ArithmeticType T, size_t N>
class FixedVector {
private:
    T data_[N];
    size_t size_ = 0;
    
public:
    ErrorCode pushBack(const T& value) {
        if (size_ >= N) {
            return ErrorCode::Full;
        }
        data_[size_++] = value;
        return ErrorCode::Success;
    }
    
    T& operator[](size_t index) {
        EMDEVIF_ASSERT(index < size_, "Index out of bounds");
        return data_[index];
    }
};
```

## 总结

概念库为 emdevif 提供了强大的类型约束机制：

- **HaveLimitType**：检查类型是否具有最大值和最小值
- **ArithmeticType**：约束算术类型，比 `std::is_arithmetic` 更广泛
- **FloatingPointType**：约束浮点类型
- **PointerType**：约束指针类型

理解这些概念的设计理念和使用场景，可以帮助编写出更安全、更高效的模板代码。