# rmdev-dev

`rmdev` 与 `emdevif` 的开发/测试集成环境仓库。

> 该仓库定位是"开发与测试环境"，不是产品工程模板。

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

# Debug 构建并运行测试（以 `HostTestModulesExceptions` 配置预设为例）
cmake --build build/mock/HostTestModulesExceptions --config Debug
ctest --test-dir build/mock/HostTestModulesExceptions -C Debug

# Release 构建并运行测试（以 `HostTestModulesExceptions` 配置预设为例）
cmake --build build/mock/HostTestModulesExceptions --config Release
ctest --test-dir build/mock/HostTestModulesExceptions -C Release
```

### 辅助脚本

推荐使用仓库提供的 PowerShell 脚本简化测试与格式化操作：

```powershell
# 获取脚本帮助信息
Get-Help ./scripts/Run-MockTests.ps1

# 一键编译并运行所有预设的单元测试（Release）
./scripts/Run-MockTests.ps1

# 仅运行指定预设（可组合短名或长名）
./scripts/Run-MockTests.ps1 -TestSuit "ME", "MNE" -GeneratorConfig Debug

# 格式化仓库内所有 C++ 代码（需安装 clang-format）
./scripts/Format-CppCode.ps1
```
