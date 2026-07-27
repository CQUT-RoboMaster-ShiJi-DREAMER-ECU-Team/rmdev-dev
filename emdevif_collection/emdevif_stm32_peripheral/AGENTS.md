# AGENTS.md

## 模块定位
`emdevif` 的 STM32 外设扩展，将 CubeMX 生成的 HAL/LL 驱动封装为 `emdevif_peripheral` 兼容的统一接口。

## 仓库结构

```
emdevif_stm32_peripheral/
├── inc/emdevif/stm32_peripheral/
│   ├── hal/                 # HAL 封装头文件（如 usart.hpp, can.hpp）
│   └── ll/                  # LL 封装头文件（如 usart.hpp）
├── STM32_HAL_Driver/        # HAL 封装实现（.cppm 模块文件）
├── STM32_LL_Driver/         # LL 封装实现（.cppm 模块文件，后续支持）
└── CMakeLists.txt
```

### 文件命名约定
外设名在 CMake 配置中使用大写（`USART`, `CAN`, `SPI`），CMake 脚本自动转换为小写文件名：

- 头文件：`inc/emdevif/stm32_peripheral/{hal|ll}/{外设小写}.hpp`
- 实现文件：`STM32_{HAL|LL}_Driver/{外设小写}_{hal|ll}_impl.cppm`

## 编码规范
统一编码规范位于 `rmdev-dev` 仓库的 `docs/coding-style.md`：

- 集成环境内（本仓库作为 `rmdev-dev` 子模块检出）：直接阅读 `../../docs/coding-style.md`（相对本仓库根目录）
- 独立查看：https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/docs/coding-style.md

## 构建系统

### 关键 CMake 变量

| 变量 | 默认 | 说明 |
|---|---|---|
| `EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST` | `""` | 启用的外设列表（大写，分号分隔），如 `"USART;CAN;SPI"` |
| `EMDEVIF_STM32_PERIPHERAL_DRIVER` | `""` | 与上表逐项对应，取 `HAL` / `LL` / `BOTH`，如 `"HAL;LL;BOTH"` |

两列表长度必须一致，按位置一一对应。CMake 脚本 `chooseFilesToInclude` 函数负责将列表映射到实际源文件。

### 库类型差异
- `EMDEVIF_USE_CPP_MODULES=ON`：构建为 **STATIC** 库，包含 `.cppm` 模块实现文件
- `EMDEVIF_USE_CPP_MODULES=OFF`：构建为 **INTERFACE** 库，仅暴露头文件

### 依赖
- `emdevif_core`（必须）
- `emdevif_stm32cubemx_support`（由 `EMDEVIF_USE_STM32CUBEMX` 引入）
- `emdevif_peripheral_model`（外设模型层）

## 开发约束
- `EMDEVIF_DEVICE_ENABLED_PERIPHERAL_LIST` 与 `EMDEVIF_STM32_PERIPHERAL_DRIVER` 必须逐项对应。
- 外设名保持大写输入约定，CMake 自动转为小写，不破坏既有映射路径。
- 仅做抽象适配，不修改 CubeMX 生成代码策略。
- HAL/LL/BOTH 三种驱动选择路径的封装接口语义需保持一致。

## 扩展指引

### 新增外设封装（以 I2C 为例）
1. 创建 HAL 头文件 `inc/emdevif/stm32_peripheral/hal/i2c.hpp`
2. 创建 HAL 实现文件 `STM32_HAL_Driver/i2c_hal_impl.cppm`（模块路径下 `EMDEVIF_USE_CPP_MODULES=ON` 时需要）
3. 如需 LL 封装，重复以上步骤，替换 `hal` 为 `ll`
4. 无需修改 CMakeLists.txt — 只要文件名符合约定，CMake 自动匹配
5. 在 AGENTS.md 中补充外设特定的约束（如有）

### 新增 HAL/LL 之外的驱动类型
需要在根 `CMakeLists.txt` 的 `chooseFilesToInclude` 函数中新增分支逻辑。

## 验证
- 检查 HAL/LL/BOTH 三种选择路径均可正确选源并编译。
- 新增外设封装后，验证至少一种驱动类型（HAL/LL/BOTH）+ `EMDEVIF_USE_CPP_MODULES=ON/OFF` 组合。
- 与 `peripheral_handle_map` 的键名交互需做端到端验证。

## 参考资料
- 编码规范：见上文「编码规范」章节（集成环境内读 `../../docs/coding-style.md`）
- 完整使用指南：`README.md`
- 外设映射逻辑：根 `CMakeLists.txt` 中 `chooseFilesToInclude` 函数
