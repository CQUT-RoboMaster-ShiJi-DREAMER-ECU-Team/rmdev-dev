# rmdev_developing_or_testing_environment

rmdev 的开发与测试环境

为了便于 [rmdev](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev)
以及 [emdevif](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif.git) 的开发与测试而创建的仓库。

由于 rmdev 以及 emdevif 都是库，没有办法直接运行，因此需要一个环境来进行开发与测试。

**本仓库重在开发与测试，并不适合作为最终产品的代码仓库使用，也不建议参考本仓库的结构来组织您的工程。**

## 路径说明

* `rmdev`、`emdevif_collection/*`: 两个子模块分别为 rmdev 以及 emdevif 的代码仓库。
* `rmdev_test`: 运行 rmdev 的测试的工程（与平台无关的测试）。
* `Platforms`: 存放各大平台的工程，内部以该平台下的不同开发板进行划分，每个开发板下包含该开发板的工程代码以及主函数。
* 其他: 第三方库或脚本。

通俗地讲，`rmdev_test` 存放各个平台共有的逻辑，而 `Platforms` 下的各个开发板路径内则存放与具体平台相关的代码。

## 配置

| CMake 缓存变量         | 类型     | 默认值  | 说明                               |
|--------------------|--------|------|----------------------------------|
| RMDEV_ENABLE_TESTS | Bool   | ON   | 是否启用 rmdev 的测试工程                 |
| PLATFORM_NAME      | String | `""` | 平台名称，取值在 `Platforms` 路径中的文件夹名称中选 |
| BOARD_NAME         | String | `""` | 开发板名称，取值在所选平台路径中的开发板的文件夹名称中选     |

## 目前支持的平台

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
