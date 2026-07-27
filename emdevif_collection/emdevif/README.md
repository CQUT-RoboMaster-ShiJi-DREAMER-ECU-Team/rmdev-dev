# emdevif

emdevif: embedded developing interface 嵌入式开发接口。

## 子模块

* [core](./core) 核心部分，所有库都需要链接它。
* [logger](./logger) 日志。
* [timeline](./timeline) 微秒级时间戳。
* [peripherals](./peripheral) 对外设的操作接口。
* [system](./system) 对操作系统（例如 FreeRTOS）调用的封装。

## 外部模块

* [emdevif_stm32_peripheral](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif_stm32_peripheral.git)
  提供一部分 STM32 HAL/LL 外设库的封装
* [emdevif_test_framework](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif_test_framework.git) 单元测试框架

## 相关文档

- [编码规范](../../docs/coding-style.md)（在 `rmdev-dev` 集成环境内；独立查看见 [GitHub](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev-dev/blob/main/docs/coding-style.md)）
- [rmdev](https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/rmdev.git)

## 模块与头文件说明

rmdev、emdevif 库提供了两种导入方式：模块导入，和头文件导入。这两种方式根据您的条件来选择使用。模块导入需要您的编译器支持 C++20 模块，并且需要在 CMake 中启用模块支持（`set(CMAKE_CXX_SCAN_FOR_MODULES ON CACHE INTERNAL "" FORCE)`），它可以提供更好的编译性能和更清晰的接口；头文件导入则兼容性、LSP 更友好，但可能会导致较长的编译时间，并且会暴露部分实现细节。

条件允许的情况下，我们建议您使用模块导入的方式，这样可以获得更好的开发体验和编译性能。

经测试，目前支持模块导入的工具链包括：

* CMake >= 3.28
* arm-none-eabi-g++ >= 14.2

经测试，目前对模块支持较好的开发环境只有启用了 Nova 引擎的 CLion 2024.2 以及之后的版本，其他 IDE（例如 Visual
Studio、VSCode）可能存在一些问题，例如无法正确识别模块接口、无法正确解析模块间的依赖关系等。

**在启用模块的情况下，请勿对同一库同时以 `import` 和 `#include` 方式引入**（除了文档明确说明的特殊情况），否则可能导致 ODR 或编译失败。整个项目可以在不同目标间分别选择 `import` 或 `#include`，但建议通过统一的 `EMDEVIF_USE_CPP_MODULES` 开关保持一致。

模块的命名与头文件的命名方式完全一致，例如：

```C++
import emdevif.core.error_handler;
// 等价于
#include "emdevif/core/error_handler.hpp"
```

在文档中，我们统一以模块导入的方式来介绍接口的使用方法，但您完全可以将模块导入替换为头文件导入，接口的使用方法完全相同。

## 使用方法

由于此项目采用了比较分散的模块化设计，您可以根据需要选择性地使用其中的某些模块，而未使用的模块完全不会参与编译。
但这样也导致了使用相对比较复杂。下面将介绍如何在您的工程中使用 emdevif。

### 基本配置结构

假如您的工程目录如下：

```
project_root
├── inc/
│   └── ...
├── src/
│   └── ...
├── CMakeLists.txt
└── ...
```

您可以使用 `git clone` 或 `git submodule add` 将本仓库添加到一个合适的目录中，例如：

```Shell
git submodule add https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team/emdevif.git emdevif_collection/emdevif
```

这样，文件结构将会变成：

```
project_root
├── inc/
│   └── ...
├── src/
│   └── ...
├── CMakeLists.txt
├── emdevif_collection/
│   └── emdevif/
│       └── ...
└── ...
```

假如您的 CMakeLists.txt 内容如下：

```CMake
cmake_minimum_required(VERSION 3.28)
project(your_project_name C CXX)

add_executable(${PROJECT_NAME}
    src/main.cpp
    # 其他源文件...
)
target_include_directories(${PROJECT_NAME} PRIVATE
    inc
    # 其他头文件路径...
)
```

我们将以这个结构为例，逐步介绍如何使用 emdevif。

首先，参考子模块列表，决定您需要哪些功能。除了 core 模块，其他模块均为可选模块。
假如您需要使用 logger 和 util 模块，则需要在 CMakeLists.txt 中设置相应的选项：

```CMake
cmake_minimum_required(VERSION 3.28)
project(your_project_name C CXX)

set(CMAKE_C_STANDARD 11)            # 最低限制是 C11
set(CMAKE_CXX_STANDARD 20)          # 最低限制是 C++20
set(CMAKE_CXX_SCAN_FOR_MODULES ON)  # 启用 C++ 模块支持（如果需要使用模块，则必须启用）

add_executable(${PROJECT_NAME}
    src/main.cpp
    # 其他源文件...
)
target_include_directories(${PROJECT_NAME} PRIVATE
    inc
    # 其他头文件路径...
)

# 您需要设置这个变量来启用所需的模块
set(EMDEVIF_ENABLED_MODULES
    "logger;util"  # 使用分号分隔模块名称（不需要手动增加 core 模块，因为它必定会被使用）
    CACHE INTERNAL "" FORCE
)  # 您也可以通过命令行传递这个变量，例如: cmake -DEMDEVIF_ENABLED_MODULES="logger;util" ...

add_subdirectory(emdevif_collection/emdevif)  # 请注意，这一条语句需要放在设置变量之后

target_link_libraries(${PROJECT_NAME} PRIVATE emdevif)  # 链接 emdevif 库
```

这样基本上就大功告成了！不过有些子模块还包含一些其他的配置选项，您可以参考 `docs/` 目录下各个子模块的文档来进行配置。

我们建议您单独创建一个 `emdevif_config.cmake` 文件来管理 emdevif 的配置选项，这样可以使得您的
CMakeLists.txt 更加清晰，并且便于管理子模块的配置。例如：

```
project_root
├── cmake/  # 存放 CMake 配置文件的目录
│   ├── emdevif_config.cmake
│   └── ...
├── inc/
│   └── ...
├── src/
│   └── ...
├── CMakeLists.txt
├── emdevif_collection/
│   └── emdevif/
│       └── ...
└── ...
```

然后修改您的 CMakeLists.txt 如下：

```CMake
# project_root/CMakeLists.txt

cmake_minimum_required(VERSION 3.28)
project(your_project_name C CXX)

set(CMAKE_C_STANDARD 11)            # 最低限制是 C11
set(CMAKE_CXX_STANDARD 20)          # 最低限制是 C++20
set(CMAKE_CXX_SCAN_FOR_MODULES ON)  # 启用 C++ 模块支持（如果需要使用模块，则必须启用）

add_executable(${PROJECT_NAME}
    src/main.cpp
    # 其他源文件...
)
target_include_directories(${PROJECT_NAME} PRIVATE
    inc
    # 其他头文件路径...
)

include(cmake/emdevif_config.cmake)  # 包含配置文件
target_link_libraries(${PROJECT_NAME} PRIVATE emdevif)
```

```CMake
# cmake/emdevif_config.cmake

# 按照相同的方式设置所需的模块
set(EMDEVIF_ENABLED_MODULES "logger;util" CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/emdevif_collection/emdevif)

# 尽管这样看上去有些多余，但如果子模块的配置选项较多，这样做会使得 CMakeLists.txt 更加清晰。
```

在 `docs/` 目录下的子模块文档中，将会提供 cmake/emdevif_config.cmake 的示例，您可以将其内容复制到您的配置文件中并加以修改即可。

### 链接期注入（timeline / peripheral / logger）

部分子模块需要用户提供底层实现，例如 timeline 提供时间戳、logger 输出日志、peripheral 查找外设句柄。这些实现通过链接期注入机制提供：用户只需在任意 `.cpp` 文件中，于命名空间 `emdevif::user_impl` 中定义所需函数，链接器自动解析符号。不再需要创建 `emdevif_user_declares` 目标。详见 `docs/` 目录下各子模块文档。

#### 示例

假设您需要在项目中使用 logger 模块（同步/异步模式），在某个 `.cpp` 文件中提供实现：

```C++
// user_impl_logger.cpp
#include <cstddef>
#include "emdevif/core/error_handler.hpp"

namespace emdevif::user_impl::logger {

std::size_t getTimeLine() noexcept
{
    // 返回时间戳
}

emdevif::ErrorCode printLogMessage(const char* message) noexcept
{
    // 输出日志消息，例如串口、控制台等
}

}
```

编译并链接该 `.cpp` 到您的目标即可。timeline 与 peripheral 的使用方式类似，具体函数签名请参考 `docs/` 目录下对应模块的文档。

### CMake 变量配置

| CMake 变量                | 类型     | 默认值                              | 说明                            |
|-------------------------|--------|----------------------------------|-------------------------------|
| EMDEVIF_USE_CPP_MODULES | Bool   | ON                               | 是否使用 C++ 模块支持（如果需要使用模块，则必须启用） |
| EMDEVIF_ENABLE_TESTS    | Bool   | OFF                              | 是否启用测试                        |
| EMDEVIF_USE_STM32CUBEMX | Bool   | OFF                              | 是否使用 STM32CubeMX              |
| EMDEVIF_ENABLED_MODULES | String | `${emdevifAllModules}`(默认开启所有模块) | 要使用的模块，模块名之间用分号间隔             |
