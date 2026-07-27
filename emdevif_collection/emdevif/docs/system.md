# emdevif_system

系统接口封装层，默认实现为 FreeRTOS。

## 配置选项

| CMake 缓存变量                       | 类型     | 默认值        | 说明                                                                               |
|----------------------------------|--------|------------|----------------------------------------------------------------------------------|
| EMDEVIF_THREAD_SYSTEM            | String | "FreeRTOS" | 使用的操作系统的名称                                                                       |
| EMDEVIF_FREERTOS_USE_STM32CUBEMX | Bool   | OFF        | 是否使用 STM32CubeMX 生成的 FreeRTOS 库。如果要启用，需要确保变量 `EMDEVIF_USE_STM32CUBEMX` 被设置为 `ON` |

## 使用建议

- 统一在平台层封装线程/锁/消息能力
- 上层业务只依赖 `emdevif_system` 抽象，避免绑死 OS API
