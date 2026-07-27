# emdevif_peripheral

外设库，用于外设操作。它不直接操作硬件，而是通过字符串名称映射到用户提供的外设实例，将应用层与底层硬件彻底解耦。

## 设计理念

emdevif 本身不实现任何外设驱动，只提供一套"名称查找 + 函数指针委托"的抽象框架。整个外设系统分三层：

- **Model 层**：定义每种外设的接口契约。它是一个纯数据聚合（`XxxModel::Instance`，包含 `void* handle` 和一组函数指针），与平台无关、与实现无关。
- **Handle Map 层**：通过链接期注入获取一个 `string_view → void*` 的查找函数，将字符串名称关联到 `Instance` 的地址。
- **Impl 层**：对用户透明的包装类（如 `Gpio`），构造函数通过名称查找 `Instance`，方法直接委托给函数指针。

用户的职责只有一件事：创建 `Instance` 并通过 `emdevif::user_impl::peripheral_handle_map::findHandle` 注册到外设句柄映射。之后应用代码只需用字符串名称即可获取外设，无需知道底层是 STM32 HAL、ESP-IDF 还是其他平台。

## 应用层使用

每种外设都有一个同名的操作类（如 `Gpio`、`Serial`），通过字符串名称构造即可使用：

```cpp
import emdevif.peripheral.gpio;
import emdevif.peripheral.serial;

void app_main() {
    // 通过名称获取 GPIO 实例（名称需与 peripheral_handle_map 中一致）
    Gpio led("led_green");
    led.write(Gpio::Set);       // 点亮 LED
    led.toggle();               // 翻转

    // 通过名称获取串口实例
    Serial debug_uart("debug_console");
    const uint8_t msg[] = "Hello\r\n";
    debug_uart.transmit(false, msg, Serial::max_delay);
}
```

构造时如果名称在 `peripheral_handle_map` 中不存在，会触发断言失败。

各外设的完整 API 见对应头文件：

| 外设 | 头文件（Modules 关闭时） | 模块名（Modules 开启时） |
|---|---|---|
| GPIO | `emdevif/peripheral/gpio.hpp` | `emdevif.peripheral.gpio` |
| Serial | `emdevif/peripheral/serial.hpp` | `emdevif.peripheral.serial` |
| SPI | `emdevif/peripheral/spi.hpp` | `emdevif.peripheral.spi` |
| I2C | *待实现* | *待实现* |
| CAN | `emdevif/peripheral/can.hpp` | `emdevif.peripheral.can` |
| PWM | `emdevif/peripheral/pwm.hpp` | `emdevif.peripheral.pwm` |
| Timer | `emdevif/peripheral/timer.hpp` | `emdevif.peripheral.timer` |

## user_impl 侧的链接期注入方法

该库通过链接期注入机制获取外设句柄。用户只需在任意一个自己的 `.cpp` 文件中，于命名空间 `emdevif::user_impl::peripheral_handle_map` 内实现 `findHandle` 函数，将字符串名称映射到外设实例即可。

注册示例（以 STM32 HAL 串口为例）：

```cpp
#include <string_view>

#include "emdevif/peripheral/model/serial.hpp"

constinit emdevif::SerialModel::Instance serial1{
    .handle = &huart1,
    .get_state_function = emdevif::stm32hal::uartGetState,
    .transmit_function = emdevif::stm32hal::uartTransmitInterrupt};

namespace emdevif::user_impl::peripheral_handle_map {

void* findHandle(std::string_view name) noexcept
{
    if (name == "serial1") return &serial1;
    return nullptr;
}

}  // namespace emdevif::user_impl::peripheral_handle_map
```

每个外设 Model 的 `Instance` 结构体字段见对应头文件（如 `emdevif/peripheral/model/gpio.hpp`）。

## 架构分层

```
peripheral/
├── model/                        # 外设模型层（平台无关的接口定义）
│   ├── inc/emdevif/peripheral/model/  # 头文件（gpio.hpp, serial.hpp, ...）
│   └── modules/                      # C++20 模块文件（gpio_model.cppm, ...）
├── peripheral_handle_map/        # 名称→实例的映射机制
│   ├── inc/emdevif/peripheral/       # peripheral_handle_map.hpp
│   └── PeripheralHandleMap.cppm
└── peripheral_impl_/             # 外设操作实现（对用户透明的具体类型）
    ├── inc/emdevif/peripheral/       # 头文件（gpio.hpp, serial.hpp, ...）
    └── Gpio.cppm, Serial.cppm, ...
```

### Model 层（`peripheral/model/`）
定义每种外设的抽象接口。每个外设包含：

- 一个 `XxxModel` 类，其中定义嵌套的 `Instance` 结构体
- `Instance` 是一组函数指针 + `void* handle` 的纯数据聚合，是用户侧与底层实现之间的契约

以 GPIO 为例：
```cpp
class GpioModel {
public:
    struct Instance {
        void* const handle{nullptr};
        const WriteFunction write_function{nullptr};
        const ReadFunction read_function{nullptr};
        const ToggleFunction toggle_function{nullptr};
    };
};
```

### Handle Map 层（`peripheral/peripheral_handle_map/`）
提供 `PeripheralHandleMap::findHandle(name)` 静态方法，委托给用户提供的 `emdevif::user_impl::peripheral_handle_map::findHandle` 函数。

- 用户需要在任意 `.cpp` 文件中实现 `emdevif::user_impl::peripheral_handle_map::findHandle(std::string_view) -> void*`
- 该函数负责将字符串名称映射到对应的 `XxxModel::Instance*`（返回 `void*`），未找到时返回 `nullptr`
- 不再要求 `constexpr`，不再使用 `makeStaticMap` / `array_map`
- 链接器在链接期自动解析符号

### Impl 层（`peripheral/peripheral_impl_/`）
每种外设的具体操作类（如 `Gpio` 继承 `GpioModel`）：

- 构造函数接受 `std::string_view name`，通过 `PeripheralHandleMap::findHandle` 查找实例
- 方法直接将调用委托给 `Instance` 中的函数指针
- 用户通过 `Gpio gpio("gpio1")` 即可使用，无需知道底层实现细节

## 数据流：从用户注册到外设调用

```
1. 用户在任意 .cpp 中:
   constinit GpioModel::Instance gpio1{ .handle = &hw_gpio, .write_function = ... };

   namespace emdevif::user_impl::peripheral_handle_map {
       void* findHandle(std::string_view name) noexcept {
           if (name == "gpio1") return &gpio1;
           return nullptr;
       }
   }

2. 应用代码:
   Gpio gpio("gpio1");           // 构造时调用 findHandle("gpio1") 查找实例
   gpio.write(Gpio::Set);        // 委托给 gpio1.write_function(hw_gpio, ...)
```
