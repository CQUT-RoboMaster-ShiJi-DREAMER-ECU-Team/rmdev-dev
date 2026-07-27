# emdevif_stm32_peripheral

`emdevif` 的 STM32 外设扩展，提供 HAL/LL 外设封装实现。

## 依赖前提

- 已集成 `emdevif`
- 已提供 `emdevif_stm32cubemx_support` 目标（通常通过设置 `EMDEVIF_USE_STM32CUBEMX=ON` 由 emdevif 自动生成；若使用自定义 HAL 集成，也可手动提供同名目标）

## 配置项

| 变量 | 默认值 | 说明 |
|---|---:|---|
| `EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST` | `""` | 启用的外设列表（大写，分号分隔），如 `"USART;CAN;SPI"` |
| `EMDEVIF_STM32_PERIPHERAL_DRIVER` | `""` | 与上表逐项对应，取 `HAL` / `LL` / `BOTH` |

两列表的元素数量必须一致，且按位置一一对应。

## 示例

```
project_root
├── inc/
│   └── ...
├── src/
│   └── ...
├── CMakeLists.txt
├── emdevif_collection/
│   ├── emdevif/
│   │   └── ...
│   └── emdevif_stm32_peripheral/
│       └── ...
└── ...
```

```cmake
set(EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST "USART;CAN;SPI" CACHE INTERNAL "" FORCE)
set(EMDEVIF_STM32_PERIPHERAL_DRIVER "HAL;LL;BOTH" CACHE INTERNAL "" FORCE)

add_subdirectory(emdevif_collection/emdevif_stm32_peripheral)
```

## 集成建议

- 先在 CubeMX/板级工程中验证底层外设可用
- 再通过本模块向上提供统一接口
- 与 `emdevif_peripheral` 的 `peripheral_handle_map` 配套使用

## 常见问题

- 配置长度不一致：CMake 会直接报错
- 外设名大小写错误：不会匹配到实现文件
- HAL/LL 选择与底层工程不一致：会导致链接或运行异常
