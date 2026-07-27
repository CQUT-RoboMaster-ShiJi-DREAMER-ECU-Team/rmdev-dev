# 端序转换模块 (endian)

## 概述

端序转换模块提供了字节序（端序）转换功能，用于处理不同系统之间的数据格式差异。该模块的核心功能是 `byteSwap` 函数，用于反转值的字节顺序。

## 设计理念

### 嵌入式系统的端序问题

在嵌入式系统中，端序问题很常见：

1. **网络通信**：网络字节序通常是大端，而许多处理器是小端
2. **文件格式**：不同的文件格式可能使用不同的字节序
3. **外设寄存器**：某些外设寄存器可能有特定的字节序要求

### 零开销抽象

`byteSwap` 函数设计为：

- **constexpr**：可以在编译时求值
- **零运行时开销**：对于常见大小，使用内建函数或位操作
- **类型安全**：支持任意类型，不仅仅是整数

## 核心功能详解

### byteSwap 函数

```cpp
template<typename T>
constexpr T byteSwap(T value);
```

**支持的类型：**

1. **1 字节类型**：无操作，直接返回
2. **2/4/8 字节整数**：使用 `std::byteswap`（C++23）或手到位操作
3. **非整数类型**：使用 `std::bit_cast` 和 `std::ranges::reverse`

**实现策略：**

```cpp
// 1. 对于 1 字节类型
if constexpr (sizeof(T) == 1) {
    return value;
}
// 2. 对于 2/4/8 字节整数
else if constexpr (std::is_integral_v<T> && (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)) {
    // 使用 std::byteswap（C++23）或手到位操作
}
// 3. 对于其他类型
else {
    // 使用 std::bit_cast 到 std::array<std::byte, N>
    // 然后 std::ranges::reverse
    // 最后 std::bit_cast 回原类型
}
```

**约束条件：**

```cpp
template<typename T>
requires std::has_unique_object_representations_v<T>
constexpr T byteSwap(T value);
```

`std::has_unique_object_representations_v<T>` 确保类型没有填充字节，这会导致未定义行为。

### 使用示例

```cpp
// 整数转换
uint16_t big_endian = 0x1234;
uint16_t little_endian = emdevif::byteSwap(big_endian);  // 0x3412

// 浮点数转换
float f = 1.0f;
uint32_t bytes = std::bit_cast<uint32_t>(f);
uint32_t swapped = emdevif::byteSwap(bytes);
float f2 = std::bit_cast<float>(swapped);

// 编译时转换
constexpr uint32_t value = 0x12345678;
constexpr uint32_t swapped = emdevif::byteSwap(value);  // 0x78563412
```

## 最佳实践

### 1. 网络通信中的端序处理

```cpp
// 发送数据：主机字节序 -> 网络字节序
uint32_t host_value = 42;
uint32_t network_value = emdevif::byteSwap(host_value);  // 假设主机是小端
send(network_value);

// 接收数据：网络字节序 -> 主机字节序
uint32_t received = receive();
uint32_t host_value = emdevif::byteSwap(received);
```

### 2. 文件格式处理

```cpp
// 读取大端格式的文件
uint32_t readBigEndian32(std::ifstream& file) {
    uint32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return emdevif::byteSwap(value);  // 假设主机是小端
}

// 写入大端格式的文件
void writeBigEndian32(std::ofstream& file, uint32_t value) {
    uint32_t big_endian = emdevif::byteSwap(value);
    file.write(reinterpret_cast<const char*>(&big_endian), sizeof(big_endian));
}
```

### 3. 外设寄存器访问

```cpp
// 某些外设寄存器是大端格式
class BigEndianRegister {
private:
    uint32_t value_;
    
public:
    void set(uint32_t value) {
        value_ = emdevif::byteSwap(value);  // 转换为大端
    }
    
    uint32_t get() const {
        return emdevif::byteSwap(value_);  // 转换为主机字节序
    }
};
```

## 易混淆易出错的功能

### 1. has_unique_object_representations 的限制

```cpp
struct WithPadding {
    uint8_t a;
    uint32_t b;  // 可能有填充字节
};

// 错误！WithPadding 可能有填充字节
// emdevif::byteSwap(WithPadding{1, 2});

// 正确的做法：使用 #pragma pack(1) 消除填充字节
#pragma pack(push, 1)
struct WithoutPadding {
    uint8_t a;
    uint32_t b;
};
#pragma pack(pop)

auto swapped = emdevif::byteSwap(WithoutPadding{1, 2});  // OK
```

### 2. 浮点数的端序转换

```cpp
float f = 1.0f;

// 错误！直接对浮点数进行字节交换
// auto swapped = emdevif::byteSwap(f);  // 编译错误

// 正确的做法：先转换为整数
uint32_t bytes = std::bit_cast<uint32_t>(f);
uint32_t swapped_bytes = emdevif::byteSwap(bytes);
float swapped_f = std::bit_cast<float>(swapped_bytes);
```

### 3. 编译时与运行时的差异

```cpp
// 编译时：使用 constexpr 路径
constexpr uint32_t value = 0x12345678;
constexpr uint32_t swapped = emdevif::byteSwap(value);

// 运行时：可能使用内建函数
uint32_t runtime_value = 0x12345678;
uint32_t runtime_swapped = emdevif::byteSwap(runtime_value);
```

## 容易让用户感到意外的设计

### 1. 1 字节类型的无操作

```cpp
uint8_t byte = 0x12;
uint8_t swapped = emdevif::byteSwap(byte);  // 仍然是 0x12

// 这可能不是你期望的，但这是正确的
// 1 字节类型没有字节顺序
```

### 2. 不同大小的转换策略

```cpp
// 2 字节：手到位操作
uint16_t value16 = 0x1234;
uint16_t swapped16 = emdevif::byteSwap(value16);

// 4 字节：可能使用内建函数
uint32_t value32 = 0x12345678;
uint32_t swapped32 = emdevif::byteSwap(value32);

// 8 字节：可能使用内建函数
uint64_t value64 = 0x123456789ABCDEF0;
uint64_t swapped64 = emdevif::byteSwap(value64);

// 其他大小：使用 bit_cast 和 reverse
struct CustomSize {
    uint8_t data[3];
};
CustomSize custom = {{1, 2, 3}};
CustomSize swapped_custom = emdevif::byteSwap(custom);  // {{3, 2, 1}}
```

### 3. constexpr 的限制

```cpp
// 编译时：可以使用 bit_cast
constexpr uint32_t value = 0x12345678;
constexpr uint32_t swapped = emdevif::byteSwap(value);

// 运行时：可以使用任何类型
struct RuntimeType {
    uint8_t data[10];
};
RuntimeType runtime = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};
RuntimeType runtime_swapped = emdevif::byteSwap(runtime);
```

## 使用示例

### 网络协议实现

```cpp
class NetworkPacket {
private:
    uint16_t id_;
    uint32_t length_;
    uint8_t data_[1024];
    
public:
    void serialize(std::vector<uint8_t>& buffer) const {
        // 转换为网络字节序（大端）
        uint16_t net_id = emdevif::byteSwap(id_);
        uint32_t net_length = emdevif::byteSwap(length_);
        
        buffer.insert(buffer.end(), 
                     reinterpret_cast<const uint8_t*>(&net_id),
                     reinterpret_cast<const uint8_t*>(&net_id) + sizeof(net_id));
        
        buffer.insert(buffer.end(),
                     reinterpret_cast<const uint8_t*>(&net_length),
                     reinterpret_cast<const uint8_t*>(&net_length) + sizeof(net_length));
        
        buffer.insert(buffer.end(), data_, data_ + length_);
    }
    
    static NetworkPacket deserialize(const uint8_t* data, size_t size) {
        NetworkPacket packet;
        
        uint16_t net_id;
        std::memcpy(&net_id, data, sizeof(net_id));
        packet.id_ = emdevif::byteSwap(net_id);
        
        uint32_t net_length;
        std::memcpy(&net_length, data + sizeof(net_id), sizeof(net_length));
        packet.length_ = emdevif::byteSwap(net_length);
        
        std::memcpy(packet.data_, data + sizeof(net_id) + sizeof(net_length), packet.length_);
        
        return packet;
    }
};
```

### 二进制文件读写

```cpp
class BinaryFile {
private:
    std::ifstream file_;
    
public:
    uint16_t readUint16BE() {
        uint16_t value;
        file_.read(reinterpret_cast<char*>(&value), sizeof(value));
        return emdevif::byteSwap(value);  // 假设主机是小端
    }
    
    uint32_t readUint32BE() {
        uint32_t value;
        file_.read(reinterpret_cast<char*>(&value), sizeof(value));
        return emdevif::byteSwap(value);
    }
    
    void writeUint16BE(uint16_t value) {
        uint16_t big_endian = emdevif::byteSwap(value);
        file_.write(reinterpret_cast<const char*>(&big_endian), sizeof(big_endian));
    }
    
    void writeUint32BE(uint32_t value) {
        uint32_t big_endian = emdevif::byteSwap(value);
        file_.write(reinterpret_cast<const char*>(&big_endian), sizeof(big_endian));
    }
};
```

### 外设驱动

```cpp
class SpiFlash {
private:
    SpiDriver& spi_;
    
public:
    uint32_t readId() {
        uint8_t cmd = 0x9F;
        uint8_t id_bytes[3];
        
        spi_.transfer(&cmd, nullptr, 1);
        spi_.transfer(nullptr, id_bytes, 3);
        
        // Flash 返回大端格式的 ID
        uint32_t id = (id_bytes[0] << 16) | (id_bytes[1] << 8) | id_bytes[2];
        return id;  // 已经是主机字节序
    }
    
    void writeAddress(uint32_t address) {
        // Flash 期望大端格式的地址
        uint8_t addr_bytes[3];
        addr_bytes[0] = (address >> 16) & 0xFF;
        addr_bytes[1] = (address >> 8) & 0xFF;
        addr_bytes[2] = address & 0xFF;
        
        spi_.transfer(addr_bytes, nullptr, 3);
    }
};
```

## 总结

端序转换模块为嵌入式系统提供了简单、高效的字节序转换功能：

- **零开销**：编译时求值，运行时无额外开销
- **类型安全**：支持任意类型，不仅仅是整数
- **constexpr 支持**：可以在编译时进行转换

理解端序问题的本质和 `byteSwap` 函数的使用场景，可以帮助编写出正确处理不同字节序系统的代码。