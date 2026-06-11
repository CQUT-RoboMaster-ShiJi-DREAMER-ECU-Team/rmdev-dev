# rmdev-dev

`rmdev` 与 `emdevif` 的开发/测试集成环境仓库。

本仓库用于：

- 在真实板级工程上联调 `rmdev`、`emdevif`
- 运行与平台相关/无关的测试入口
- 验证跨平台构建（STM32 / ESP32）

> 该仓库定位是"开发与测试环境"，不是量产产品工程模板。

## 仓库结构

- `rmdev`、`emdevif_collection/*`：核心库子模块
- `RunningPlatforms/`：平台与板级工程（按平台/开发板划分）
- `TestImplement/`：跨平台测试实现与测试入口适配
- `Packages/`：第三方依赖（本仓库集成使用）
- `CMakePresets.json`：CMake 预设（统一管理各板卡的构建参数）

## 快速开始

### 1) 拉取子模块

```bash
git submodule update --init --recursive
```

### 2) 选择平台与开发板配置

CMake 变量：

| 变量                   | 类型     |  默认值 | 说明                                                        |
|----------------------|--------|-----:|-----------------------------------------------------------|
| `RMDEV_ENABLE_TESTS` | Bool   | `ON` | 是否启用 `rmdev` 测试。如果只测试 emdevif 而不测试 rmdev 时，可以将这个变量设置为 OFF |
| `PLATFORM_NAME`      | String | `""` | 平台名：`stm32` / `esp32`                                     |
| `BOARD_NAME`         | String | `""` | 具体开发板目录名                                                  |

### 3) 构建

**STM32（通过 CMakePresets.json）**

使用 `cmake --list-presets` 查看所有可用预设，然后选择对应板卡：

| 预设名 | 对应板卡 |
|---|---|
| `RoboMasterDevelopmentBoardTypeC` | RoboMaster 开发板 C 型 (STM32F407IGHx) |
| `ShiJiDreamerDevBoardF4` | 士继 DREAMER 开发板 (STM32F407VGT6) |
| `DM-MC-Board02` | 达妙 DM-MC-Board02 (STM32H723VGT6) |
| `F103C8T6` | STM32F103C8T6 最小系统板 |

每个 configure 预设均有对应的 `-Debug` / `-Release` build 预设。

```bash
# 配置（以 RoboMasterDevelopmentBoardTypeC 为例）
cmake --preset RoboMasterDevelopmentBoardTypeC
# 构建 Debug
cmake --build --preset RoboMasterDevelopmentBoardTypeC-Debug
# 构建 Release
cmake --build --preset RoboMasterDevelopmentBoardTypeC-Release
```

**ESP32（需已安装 ESP-IDF）**

```bash
idf.py -DPLATFORM_NAME=esp32 -DBOARD_NAME=ESP32-DevKitC build
```

## 当前支持平台

* [STM32](./RunningPlatforms/STM32)
    * [RoboMasterDevelopmentBoardTypeC](RunningPlatforms/STM32/RoboMasterDevelopmentBoardTypeC): [RoboMaster 开发板 C 型](https://www.robomaster.com/zh-CN/products/components/general/development-board-type-c)
      (STM32F407IGH6TR, 外部高速晶振频率为 12MHz)<br>
      接线：
      ```
      测试信息输出    -> USART6 - 外壳丝印为 UART1
      姿态解算结果输出 -> USART1 - 外壳丝印为 UART2
      ```
    * [DM-MC-Board02](RunningPlatforms/STM32/DM-MC-Board02): [达妙科技 DM-MC-Board02 电机开发板](https://gitee.com/kit-miao/dm-mc02)
      (STM32H723VGT6, 外部高速晶振频率为 24MHz)<br>
      接线：
      ```
      测试信息输出    -> USART10
      姿态解算结果输出 -> UART7
      ```
    * [ShiJiDreamerDevBoardF4](RunningPlatforms/STM32/ShiJiDreamerDevBoardF4): [RoboMaster 士继DREAMER 战队电控组](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team)
      设计的一款基于 STM32F4 的开发板 (STM32F407VGT6, 外部高速晶振频率为 8MHz)<br>
      接线：
      ```
      测试信息输出 -> USART1
      ```
* [ESP32](./RunningPlatforms/ESP32)
    * [ESP32-DevKitC](RunningPlatforms/ESP32/ESP32-DevKitC): [ESP32-DevKitC 型开发板](https://www.espressif.com.cn/zh-hans/products/devkits/esp32-devkitc)
      (ESP32 系列)<br>
      接线：
      ```
      下载/监视信息 -> UART0 - 接 USB 口
      测试信息输出  -> UART2 - Tx: GPIO4
      ```
