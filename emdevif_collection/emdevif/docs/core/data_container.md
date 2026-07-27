# 数据容器模块 (data_container)

## 概述

数据容器模块提供了一套嵌入式友好的数据结构，具有以下共同特点：

- **无堆分配**：所有容器使用固定容量的栈上存储
- **确定性行为**：没有动态内存分配，行为可预测
- **错误码返回**：容量不足时返回错误码，而非抛出异常
- **constexpr 支持**：大多数容器可以在编译时使用

该模块包含六个组件：

1. **RingBuffer** - 环形缓冲区
2. **BasicFixedString** - 编译时固定字符串
3. **StaticMap** - 编译时静态映射
4. **MessageSlot/MessageQueue** - 消息队列概念
5. **InplaceDynamicArray** - 固定容量动态数组
6. **InplaceMap** - 固定容量有序映射

## 设计理念

### 嵌入式环境的数据结构约束

在嵌入式系统中，传统的 STL 容器（如 `std::vector`、`std::map`）存在以下问题：

1. **堆分配**：嵌入式系统可能没有堆，或者堆空间有限
2. **异常处理**：STL 容器在分配失败时抛出异常
3. **代码大小**：模板实例化可能导致代码膨胀
4. **确定性**：动态内存分配的时间不确定

因此，emdevif 提供了固定容量的替代方案。

### 容量不足时的错误处理

所有容器在容量不足时返回 `ErrorCode`，而不是抛出异常：

```cpp
ErrorCode push(const T& item);  // 返回 ErrorCode::Full 如果满了
ErrorCode insert(const Key& key, const Value& value);  // 返回 ErrorCode::Full 如果满了
```

## 核心组件详解

### RingBuffer 环形缓冲区

`RingBuffer` 是一个固定大小的循环缓冲区，适用于消息队列、串口接收缓冲区等场景。

```cpp
// 创建一个最多存储 16 个 uint8_t 的环形缓冲区
emdevif::RingBuffer<uint8_t, 16> buffer;

// 写入数据
buffer.push(0x01);
buffer.push(0x02);

// 读取数据
auto value = buffer.pop();  // 返回 0x01
```

**关键特性：**

- **FIFO 驱逐**：当缓冲区满时，`push` 会覆盖最旧的元素
- **零拷贝模式**：支持直接访问内部缓冲区
- **批量操作**：支持 `push(span)` 和 `pop(span)` 批量操作

**零拷贝写入模式：**

```cpp
// 获取下一个写入槽位的引用
auto& slot = buffer.nextSlot();
// 直接写入槽位
slot = constructData();
// 标记槽位已使用
buffer.useNextSlot();
```

**零拷贝读取模式：**

```cpp
// 获取尾部元素的引用
auto& item = buffer.peekRef();
// 处理元素
process(item);
// 丢弃已处理的元素
buffer.discard();
```

**易混淆点：**

1. **FIFO 驱逐**：`push` 在缓冲区满时不会返回错误，而是覆盖最旧的元素
2. **pop 的未定义行为**：在空缓冲区上调用 `pop` 是未定义行为
3. **RingUnsigned 的减法**：`lhs - rhs` 返回顺时针距离，不是算术差

### BasicFixedString 固定字符串

`BasicFixedString` 是一个编译时固定字符串，可用作非类型模板参数（NTTP）。

```cpp
// 创建固定字符串
constexpr emdevif::FixedString<5> str("Hello");

// 用作模板参数
template<emdevif::FixedString Name>
struct NamedResource {
    static constexpr auto name = Name;
};

NamedResource<"sensor"> resource;
```

**关键特性：**

- **编译时构造**：所有构造函数都是 `consteval`
- **NTTP 支持**：可以用作模板参数
- **编译时操作**：支持连接、比较等操作

**编译时连接：**

```cpp
constexpr auto hello = emdevif::FixedString<5>("Hello");
constexpr auto world = emdevif::FixedString<5>(" world");
constexpr auto hello_world = hello + world;  // 编译时连接
```

**易混淆点：**

1. **大小参数**：`FixedString<N>` 存储 `N+1` 个字符（包括空终止符）
2. **consteval 构造**：C 字符串构造函数只能在编译时使用
3. **公共数据成员**：`data_` 是公共的，以支持聚合初始化

### StaticMap 静态映射

`StaticMap` 是一个编译时静态映射，适用于查找表、配置映射等场景。

```cpp
// 创建编译时映射（使用模板参数指定键值类型，避免逐个写 std::pair）
constexpr auto error_names = emdevif::makeStaticMap<ErrorCode, std::string_view>({
    {ErrorCode::Success, "OK"},
    {ErrorCode::Timeout, "Timeout"},
    {ErrorCode::InvalidArgument, "Invalid argument"}
});

// 查找
auto name = error_names.at(ErrorCode::Timeout);  // 返回 "Timeout"
auto missing = error_names.at(ErrorCode::NotFound);  // 返回 nullptr
```

**关键特性：**

- **编译时构造**：所有构造函数都是 `consteval`
- **线性查找**：使用 `std::ranges::find` 进行查找
- **安全访问**：`at()` 返回指针，`operator[]` 断言存在

**易混淆点：**

1. **线性查找**：O(n) 复杂度，不适用于大型映射
2. **operator[] 的断言**：如果键不存在，在发布版本中是未定义行为
3. **不允许重复键**：构造时会断言检查重复键

### MessageSlot/MessageQueue 概念

这两个概念定义了 RTOS 消息队列的接口，不提供实际实现。

```cpp
// MessageSlot 概念：基本的消息槽
template<typename T>
concept MessageSlot = requires(T& slot) {
    typename T::ValueType;
    { T::item_size } -> std::convertible_to<size_t>;
    { slot.forcePush(std::declval<typename T::ValueType>()) } -> std::same_as<ErrorCode>;
    { slot.peek() } -> std::same_as<ErrorCode>;
    // ...
};

// MessageQueue 概念：扩展了阻塞操作
template<typename T>
concept MessageQueue = MessageSlot<T> requires(T& queue) {
    { queue.push(std::declval<typename T::ValueType>(), bool{}, 0) } -> std::same_as<ErrorCode>;
    { queue.pop(std::declval<typename T::ValueType&>(), bool{}, 0) } -> std::same_as<ErrorCode>;
};
```

**设计哲学：**

- 只定义接口，不定义实现
- 实际的队列实现在 `system/` 模块（如 FreeRTOS 包装）
- 所有操作都接受 `in_isr` 参数，用于中断上下文安全

### InplaceDynamicArray 固定容量动态数组

`InplaceDynamicArray` 是一个固定容量的动态数组，类似于 C++26 的 `std::inplace_vector`。

```cpp
// 创建一个最多存储 10 个 int 的动态数组
emdevif::InplaceDynamicArray<int, 10> arr;

// 添加元素
arr.pushBack(1);
arr.pushBack(2);
arr.pushBack(3);

// 访问元素
int first = arr[0];  // 1
int* ptr = arr.at(5);  // nullptr，因为索引越界
```

**关键特性：**

- **手动存储管理**：使用 `alignas(T) unsigned char store_[sizeof(T) * N]`
- **错误码返回**：容量不足时返回 `ErrorCode`
- **STL 兼容接口**：提供迭代器、`pushBack`、`insert`、`erase` 等

**三种 push 变体：**

```cpp
ErrorCode pushBack(const T& item);  // 返回 ErrorCode::Full 如果满了
void uncheckedPushBack(const T& item);  // 不检查容量（UB 如果满了）
T* tryPushBack(const T& item);  // 返回指针或 nullptr
```

**易混淆点：**

1. **operator[] 的断言**：越界访问在发布版本中是未定义行为
2. **at() 返回指针**：失败时返回 `nullptr`，而不是抛出异常
3. **front()/back() 的断言**：空数组上调用是未定义行为

### InplaceMap 固定容量有序映射

`InplaceMap` 是一个固定容量的有序映射，使用两个独立的数组存储键和值。

```cpp
// 创建一个最多存储 5 个键值对的映射
emdevif::InplaceMap<int, const char*, 5> map;

// 插入
map.insert(1, "one");
map.insert(2, "two");

// 查找
auto* value = map.at(1);  // 返回 "one"
auto* missing = map.at(3);  // 返回 nullptr
```

**关键特性：**

- **分离存储**：键和值分别存储在两个数组中，提高缓存友好性
- **有序维护**：使用 `std::lower_bound` 维护有序性
- **虚拟对迭代器**：迭代器解引用返回 `std::pair<const Key&, T&>`

**关键操作：**

```cpp
// 插入或更新
map.insertOrAssign(1, "new_one");

// 获取或创建
auto* value = map.getOrCreate(4);  // 如果不存在，创建新条目

// 范围查询
auto [lower, upper] = map.equalRange(2);
```

**易混淆点：**

1. **operator[] 的断言**：键不存在时在发布版本中是未定义行为
2. **getOrCreate 的失败**：映射满时返回 `nullptr`
3. **迭代器的开销**：迭代器存储指向映射的指针和索引，比原始指针稍大

## 最佳实践

### 1. 选择合适的容器

```cpp
// 消息队列 -> RingBuffer
emdevif::RingBuffer<Message, 16> messageQueue;

// 编译时查找表 -> StaticMap
constexpr auto config = emdevif::makeStaticMap({...});

// 运行时动态数组 -> InplaceDynamicArray
emdevif::InplaceDynamicArray<SensorData, 100> readings;

// 运行时有序映射 -> InplaceMap
emdevif::InplaceMap<int, CalibrationData, 10> calibration;
```

### 2. 处理容量不足

```cpp
emdevif::InplaceDynamicArray<int, 5> arr;

// 好的做法：检查返回值
auto result = arr.pushBack(42);
if (result == ErrorCode::Full) {
    // 处理数组已满的情况
    handleOverflow();
}

// 或者使用 tryPushBack
if (int* ptr = arr.tryPushBack(42)) {
    // 成功，ptr 指向新元素
} else {
    // 失败，数组已满
}
```

### 3. 使用零拷贝模式

```cpp
emdevif::RingBuffer<LargeStruct, 8> buffer;

// 零拷贝写入
auto& slot = buffer.nextSlot();
slot.field1 = value1;
slot.field2 = value2;
buffer.useNextSlot();

// 零拷贝读取
auto& item = buffer.peekRef();
process(item);
buffer.discard();
```

### 4. 编译时计算

```cpp
// 编译时创建查找表
constexpr auto lookup_table = emdevif::makeStaticMap({
    std::pair{0, "zero"},
    std::pair{1, "one"},
    std::pair{2, "two"}
});

// 编译时字符串操作
constexpr auto prefix = emdevif::FixedString<3>("pre");
constexpr auto suffix = emdevif::FixedString<3>("fix");
constexpr auto full = prefix + suffix;  // "prefix"
```

## 易混淆易出错的功能

### 1. RingBuffer 的 FIFO 驱逐

```cpp
emdevif::RingBuffer<int, 3> buffer;
buffer.push(1);
buffer.push(2);
buffer.push(3);  // 缓冲区满
buffer.push(4);  // 覆盖 1，缓冲区现在是 [2, 3, 4]

auto value = buffer.pop();  // 返回 2，不是 1
```

### 2. FixedString 的大小参数

```cpp
// FixedString<5> 存储 6 个字符（5 + 空终止符）
emdevif::FixedString<5> str("Hello");  // OK
// emdevif::FixedString<5> str("Hello!");  // 错误！6 个字符 + 空终止符
```

### 3. InplaceMap 的分离存储

```cpp
emdevif::InplaceMap<int, int, 5> map;
map.insert(1, 10);
map.insert(2, 20);

// 键和值分别存储在两个数组中
// keys:   [1, 2, ?, ?, ?]
// values: [10, 20, ?, ?, ?]

// 迭代器返回虚拟对
for (auto& [key, value] : map) {
    // key 是 keys_ 数组中元素的引用
    // value 是 values_ 数组中元素的引用
}
```

### 4. StaticMap 的线性查找

```cpp
// 对于大型映射，线性查找可能较慢
constexpr auto large_map = emdevif::makeStaticMap({
    // 100 个条目...
});

// 查找复杂度是 O(n)
auto* value = large_map.at(key);
```

## 容易让用户感到意外的设计

### 1. RingBuffer 的 pop 未定义行为

```cpp
emdevif::RingBuffer<int, 5> buffer;
// 缓冲区为空
auto value = buffer.pop();  // 未定义行为！
// 应该先检查 buffer.isEmpty() 或 buffer.count() > 0
```

### 2. InplaceDynamicArray 的 operator[] 断言

```cpp
emdevif::InplaceDynamicArray<int, 5> arr;
arr.pushBack(1);

// 调试模式：断言失败
int value = arr[5];  // 断言失败

// 发布模式：未定义行为
int value = arr[5];  // 可能返回垃圾值
```

### 3. StaticMap 的 operator[] 断言

```cpp
constexpr auto map = emdevif::makeStaticMap({
    std::pair{1, "one"},
    std::pair{2, "two"}
});

// 调试模式：断言失败
auto* value = map[3];  // 断言失败

// 发布模式：未定义行为
auto* value = map[3];  // 可能返回垃圾值
```

### 4. InplaceMap 的 getOrCreate 失败

```cpp
emdevif::InplaceMap<int, int, 2> map;
map.insert(1, 10);
map.insert(2, 20);  // 映射已满

auto* value = map.getOrCreate(3);  // 返回 nullptr，不是创建新条目
```

## 使用示例

### 串口接收缓冲区

```cpp
class UartReceiver {
private:
    emdevif::RingBuffer<uint8_t, 256> rxBuffer;
    
public:
    void onByteReceived(uint8_t byte) {
        // 中断上下文中调用
        rxBuffer.push(byte);  // 如果满，覆盖最旧的字节
    }
    
    size_t read(uint8_t* buffer, size_t maxLen) {
        size_t count = 0;
        while (count < maxLen && !rxBuffer.isEmpty()) {
            buffer[count++] = rxBuffer.pop();
        }
        return count;
    }
};
```

### 传感器数据记录

```cpp
struct SensorReading {
    uint32_t timestamp;
    float temperature;
    float humidity;
};

class DataLogger {
private:
    emdevif::InplaceDynamicArray<SensorReading, 1000> readings;
    
public:
    ErrorCode record(const SensorReading& reading) {
        return readings.pushBack(reading);
    }
    
    void upload() {
        for (const auto& reading : readings) {
            sendReading(reading);
        }
        readings.clear();
    }
};
```

### 配置管理

```cpp
enum class ConfigKey {
    BaudRate,
    SampleRate,
    Threshold
};

class ConfigManager {
private:
    emdevif::InplaceMap<ConfigKey, uint32_t, 10> config;
    
public:
    void setDefault() {
        config.insert(ConfigKey::BaudRate, 115200);
        config.insert(ConfigKey::SampleRate, 1000);
        config.insert(ConfigKey::Threshold, 100);
    }
    
    uint32_t get(ConfigKey key) const {
        auto* value = config.at(key);
        return value ? *value : 0;
    }
    
    ErrorCode set(ConfigKey key, uint32_t value) {
        return config.insertOrAssign(key, value);
    }
};
```

### 错误码名称查找

```cpp
constexpr auto error_names = emdevif::makeStaticMap({
    std::pair{ErrorCode::Success, "Success"},
    std::pair{ErrorCode::UnknownError, "Unknown error"},
    std::pair{ErrorCode::InvalidArgument, "Invalid argument"},
    std::pair{ErrorCode::OutOfMemory, "Out of memory"},
    std::pair{ErrorCode::Timeout, "Timeout"}
});

const char* getErrorName(ErrorCode code) {
    auto* name = error_names.at(code);
    return name ? *name : "Unknown";
}
```

## 总结

数据容器模块为嵌入式环境提供了一套完整的数据结构解决方案：

- **RingBuffer**：高效的消息队列，支持零拷贝操作
- **FixedString**：编译时字符串，支持 NTTP
- **StaticMap**：编译时查找表，适用于配置映射
- **InplaceDynamicArray**：固定容量的动态数组，替代 `std::vector`
- **InplaceMap**：固定容量的有序映射，替代 `std::map`

理解这些容器的设计理念和潜在陷阱，可以帮助编写出更高效、更可靠的嵌入式代码。