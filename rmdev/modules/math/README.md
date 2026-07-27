# math

`rmdev` 数学工具模块，提供基础数学能力与（可选）CMSIS-DSP 矩阵加速能力。

## 依赖

- `emdevif_core`
- 可选：`CMSISDSP`

## 配置

- `RMDEV_MATH_USE_CMSIS_DSP`（默认 `ON`）
  - `ON`：链接 `CMSISDSP` 并启用相关实现
  - `OFF`：仅使用纯 C++ 路径

- 宏 `RMDEV_FLOAT_EQU_DEFAULT_ERROR_VALUE`（默认 `0.001f`）
  - 浮点“近似相等”判定误差阈值
