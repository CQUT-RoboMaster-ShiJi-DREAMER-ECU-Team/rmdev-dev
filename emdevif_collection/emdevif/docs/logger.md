# emdevif_logger

日志模块，支持同步/异步/外部实现等模式。

## 依赖

- `emdevif_core`
- `emdevif_system`（异步模式或同步+锁时，需设置 CMake 变量 `EMDEVIF_LOGGER_LINK_SYSTEM_LIB` 为 `ON`）

参考 [emdevif_logger_config_example.hpp](../logger/emdevif_logger_config_example.hpp)
文件，您可以将这个文件拷贝到您的工程中，并根据需要修改配置选项。然后在 CMakeLists.txt 中定义
`EMDEVIF_LOGGER_CONFIG_FILE` 宏来指定配置文件：

```CMake
target_compile_definitions(emdevif_logger PUBLIC
    EMDEVIF_LOGGER_CONFIG_FILE="path/to/your/emdevif_logger_config.hpp"
    # 注意：请确保宏定义的值被双引号包裹，从而让它被正确地解析为字符串字面量
)
```

## 日志模式选择

通过 CMake 缓存变量 `EMDEVIF_LOGGER_MODE` 选择日志模式：

- `0`：同步模式（Sync）
- `1`：异步模式（Async）
- `2`：外部实现模式（ExternalImpl）

默认值为 `0`（同步模式）。

选定的模式对应的库目标（`emdevif_logger_sync` / `emdevif_logger_async` / `emdevif_logger_external`）会被构建并别名为 `emdevif_logger`。只有被选中的目标会被构建，其他模式的代码不会被编译。

## 链接期注入（user_impl）

logger 模块通过链接期注入机制让用户提供底层实现，您只需在任意 `.cpp` 文件中，于命名空间 `emdevif::user_impl::logger` 中定义所需函数，链接器自动解析符号。

### 同步/异步模式

在同步或异步模式下，您需要提供以下函数：

```C++
#include <cstddef>
#include "emdevif/core/error_handler.hpp"

namespace emdevif::user_impl::logger {

std::size_t getTimeLine() noexcept
{
    // 返回时间戳，单位可以是毫秒、微秒等，取决于具体实现
}

emdevif::ErrorCode printLogMessage(const char* message) noexcept
{
    // 在这里实现日志消息的打印逻辑，例如输出到控制台、文件、串口外设等
    // 返回 emdevif::ErrorCode::Success 表示成功，其他值表示失败
}

}
```

**注意：** 不应在这些实现中链接 `emdevif_logger` 或导入 `emdevif.logger` 模块，否则导致循环依赖。

### 外部实现模式

在外部实现模式下，您需要提供每级别的日志实现函数（无 `init`/`deInit`，用户自行初始化/反初始化外部日志库）：

```C++
#include <cstdarg>
#include "emdevif/core/error_handler.hpp"

namespace emdevif::user_impl::logger {

void logImpl(const int level, const char* format, std::va_list args) noexcept
{
    // 实现日志输出，参数 format 和可变参数列表与 printf 风格的格式化函数相同
    // level 参数表示日志级别，从 Verbose 到 Fatal 为 0 到 5 (详见 emdevif::logger::LoggerLevel 枚举)
    // 您需要自行实现 emdevif::logger::LoggerLevel 与外部日志系统日志级别之间的映射关系
}

void logVerboseImpl(const char* format, std::va_list args) noexcept
{
    // 实现 Verbose 级别日志输出
}

void logDebugImpl(const char* format, std::va_list args) noexcept
{
    // 实现 Debug 级别日志输出
}

void logInfoImpl(const char* format, std::va_list args) noexcept
{
    // 实现 Info 级别日志输出
}

void logWarningImpl(const char* format, std::va_list args) noexcept
{
    // 实现 Warning 级别日志输出
}

void logErrorImpl(const char* format, std::va_list args) noexcept
{
    // 实现 Error 级别日志输出
}

void logFatalImpl(const char* format, std::va_list args) noexcept
{
    // 实现 Fatal 级别日志输出
}

}
```

## 示例

### 同步/异步模式示例

```C++
#include <cstdio>

import emdevif.logger;

int main()
{
    emdevif::logger::init(std::vsnprintf);

    while (true) {
        using namespace emdevif;
        logger::info("Hello, emdevif Logger!");
        logger::info("Support printf-style %s!", "format");
    }
}
```

### 外部实现模式示例

```C++
#include <cstdio>

import emdevif.logger;

// 在您的工程的其他 .cpp 文件中提供 emdevif::user_impl::logger 的实现（见上文）

int main()
{
    // 外部实现模式无 init/deInit，您自行初始化外部日志库

    while (true) {
        using namespace emdevif;
        logger::info("Hello, emdevif Logger!");
        logger::info("Support printf-style %s!", "format");
    }
}
```