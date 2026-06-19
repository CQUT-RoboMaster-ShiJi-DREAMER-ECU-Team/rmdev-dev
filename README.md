# rmdev-dev

`rmdev` 与 `emdevif` 的开发/测试集成环境仓库。

> 该仓库定位是"开发与测试环境"，不是量产产品工程模板。

## 仓库结构

- `rmdev`、`emdevif_collection/*`：核心库子模块
- `CMakePresets.json`：CMake 预设

## 快速开始

```bash
# 列出所有可用预设
cmake --list-presets

# 配置（4 选 1）
cmake --preset HostTestModulesExceptions
cmake --preset HostTestModulesNoexceptions
cmake --preset HostTestHeadersExceptions
cmake --preset HostTestHeadersNoexceptions

# Debug 构建并运行测试
cmake --build --preset HostTestModulesExceptions --config Debug
ctest --preset HostTestModulesExceptions -C Debug

# Release 构建并运行测试
cmake --build --preset HostTestModulesExceptions --config Release
ctest --preset HostTestModulesExceptions -C Release
```
