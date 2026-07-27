# AGENTS.md

## 模块定位

`emdevif` 提供嵌入式开发通用接口抽象（core/logger/timeline/peripheral/system），并作为 `rmdev` 的底层基础能力。

## 仓库结构

```
emdevif/
├── core/                       # 必选基础层（错误处理、断言、C/C++ 边界宏、depends/）
├── logger/                     # 日志模块
├── timeline/                   # 微秒级时间戳
├── peripheral/                 # 外设操作接口（model / handle_map / impl_，有独立 AGENTS.md）
├── system/                     # RTOS 抽象（当前仅 FreeRTOS 实现）
├── support/                    # 平台适配层（stm32cubemx / esp-idf）
├── check_compile_features/     # 编译器 C++20 模块特性检测
└── cmake/                      # CMake 辅助脚本（utility.cmake）
```

## 编码规范

统一编码规范位于 `rmdev-dev` 仓库的 `docs/coding-style.md`：

- 集成环境内（本仓库作为 `rmdev-dev` 子模块检出）：直接阅读 `../../docs/coding-style.md`（相对本仓库根目录）
- 独立查看：https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/docs/coding-style.md

## 构建系统

### 关键 CMake 变量

| 变量                        | 默认    | 说明                                                |
|---------------------------|-------|---------------------------------------------------|
| `EMDEVIF_USE_CPP_MODULES` | `ON`  | C++20 模块开关。构建前由 `check_compile_features/` 验证编译器支持 |
| `EMDEVIF_ENABLED_MODULES` | 全部    | 模块裁剪列表（分号分隔），如 `"logger;system;util"`             |
| `EMDEVIF_USE_STM32CUBEMX` | `OFF` | 启用 STM32CubeMX 平台适配，与 `EMDEVIF_USE_ESP_IDF` 互斥    |
| `EMDEVIF_USE_ESP_IDF`     | `OFF` | 启用 ESP-IDF 平台适配                                   |

### 平台适配

- `support/stm32cubemx/`：启用后引入 HAL 库和 CMSIS 启动文件
- `support/esp-idf/`：启用后适配 ESP-IDF 组件体系

两个选项互斥（`cmake/utility.cmake` 中的 `emdevif_assertMutuallyExclusiveOptions` 强制检查）。

### 测试

单元测试已迁移至模拟测试环境，测试文件位于开发环境仓库 [rmdev-dev](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev) 根目录下的 `tests/mock/emdevif/`，使用 GoogleTest 框架。详见 `rmdev-dev` 根 `AGENTS.md` 的模拟单元测试章节。

## 参考资料

- 编码规范：见上文「编码规范」章节（集成环境内读 `../../docs/coding-style.md`）
- 开发环境（集成与测试）：https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev
- Peripheral 架构与扩展：`peripheral/AGENTS.md`
- 完整使用指南：`README.md`
