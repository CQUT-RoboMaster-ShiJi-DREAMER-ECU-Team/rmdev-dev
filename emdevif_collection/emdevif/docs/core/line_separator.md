# 换行符模块 (line_separator)

## 概述

换行符模块提供了一个编译时可配置的换行符字符串，使得同一个代码库可以针对不同的串口协议或文件系统使用不同的换行符风格（`\r\n`、`\n` 或 `\r`）。

## 设计理念

### 嵌入式系统的换行符问题

在嵌入式系统中，不同的设备和协议可能使用不同的换行符：

- **Windows 风格**：`\r\n`（CRLF）
- **Unix 风格**：`\n`（LF）
- **老式 Mac 风格**：`\r`（CR）

### 零运行时开销

换行符的配置完全在编译时完成，没有运行时开销：

```cpp
// 编译时配置
EMDEVIF_LINE_SEPARATOR_MODE=EMDEVIF_LINE_SEPARATOR_MODE_LF

// 编译时展开
"Hello" EMDEVIF_LINE_SEPARATOR  // 等价于 "Hello\n"
```

### CMake 集成

配置通过 CMake 的 `target_compile_definitions` 完成，确保所有依赖目标都继承相同的配置。

## 配置方法

### 可用的模式

| 宏值 | 含义 | 展开结果 |
|------|------|----------|
| `EMDEVIF_LINE_SEPARATOR_MODE_CRLF` | Windows 风格（默认） | `\r\n` |
| `EMDEVIF_LINE_SEPARATOR_MODE_LF` | Unix 风格 | `\n` |
| `EMDEVIF_LINE_SEPARATOR_MODE_CR` | 老式 Mac 风格 | `\r` |

### CMake 配置示例

```cmake
# 设置 LF 换行符
target_compile_definitions(emdevif_core PUBLIC
    EMDEVIF_LINE_SEPARATOR_MODE=EMDEVIF_LINE_SEPARATOR_MODE_LF
)

# 设置 CRLF 换行符（默认）
target_compile_definitions(emdevif_core PUBLIC
    EMDEVIF_LINE_SEPARATOR_MODE=EMDEVIF_LINE_SEPARATOR_MODE_CRLF
)
```

## 使用示例

### 基本使用

```cpp
#include <stdio.h>
#include "emdevif/core/line_separator.h"

int main() {
    printf("Hello, World!" EMDEVIF_LINE_SEPARATOR);
    // 等价于 printf("Hello, World!\r\n");

    printf("Line 1" EMDEVIF_LINE_SEPARATOR);
    printf("Line 2" EMDEVIF_LINE_SEPARATOR);

    return 0;
}
```

### 串口通信

```cpp
#include "emdevif/core/line_separator.h"

class SerialConsole {
public:
    void sendPrompt() {
        sendString("> " EMDEVIF_LINE_SEPARATOR);
    }

    void sendMessage(const char* msg) {
        sendString(msg);
        sendString(EMDEVIF_LINE_SEPARATOR);
    }

    void sendError(const char* msg) {
        sendString("ERROR: ");
        sendString(msg);
        sendString(EMDEVIF_LINE_SEPARATOR);
    }

private:
    void sendString(const char* str) {
        while (*str) {
            sendChar(*str++);
        }
    }

    void sendChar(char c) {
        // 发送单个字符
    }
};
```

### 日志系统

```cpp
#include "emdevif/core/line_separator.h"

class Logger {
public:
    void log(const char* format, ...) {
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        // 添加换行符
        sendString(buffer);
        sendString(EMDEVIF_LINE_SEPARATOR);
    }

private:
    void sendString(const char* str) {
        // 发送字符串
    }
};
```

## 最佳实践

### 1. 统一配置

```cmake
# 在项目根目录的 CMakeLists.txt 中统一配置
# 确保所有目标使用相同的换行符
set(EMDEVIF_LINE_SEPARATOR_MODE EMDEVIF_LINE_SEPARATOR_MODE_CRLF)
target_compile_definitions(emdevif_core PUBLIC
    EMDEVIF_LINE_SEPARATOR_MODE=${EMDEVIF_LINE_SEPARATOR_MODE}
)
```

### 2. 在协议实现中使用

```cpp
// 协议消息格式
void sendProtocolMessage(const char* type, const char* data) {
    sendString("MSG:");
    sendString(type);
    sendString(":");
    sendString(data);
    sendString(EMDEVIF_LINE_SEPARATOR);  // 协议要求的换行符
}
```

### 3. 与格式化函数配合

```cpp
#include <stdio.h>
#include "emdevif/core/line_separator.h"

void logFormatted(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    printf("%s" EMDEVIF_LINE_SEPARATOR, buffer);
}
```

## 易混淆易出错的功能

### 1. 字符串字面量的连接

```cpp
// EMDEVIF_LINE_SEPARATOR 是字符串字面量
// 可以与其他字符串字面量在编译时连接

"Hello" EMDEVIF_LINE_SEPARATOR  // 等价于 "Hello\r\n"

// 但不能与变量连接
const char* msg = "Hello";
msg EMDEVIF_LINE_SEPARATOR;  // 错误！
```

### 2. 配置的继承

```cmake
# emdevif_core 的配置会传播到所有依赖它的目标
target_compile_definitions(emdevif_core PUBLIC
    EMDEVIF_LINE_SEPARATOR_MODE=EMDEVIF_LINE_SEPARATOR_MODE_LF
)

# 用户目标会自动继承这个配置
add_executable(my_app main.cpp)
target_link_libraries(my_app PUBLIC emdevif_core)
# my_app 也会使用 LF 换行符
```

### 3. 默认值

```cpp
// 如果没有定义 EMDEVIF_LINE_SEPARATOR_MODE
// 默认使用 CRLF（\r\n）

// 这可能不是你期望的
// 建议在 CMakeLists.txt 中显式设置
```

## 容易让用户感到意外的设计

### 1. 编译时配置

```cpp
// 配置在编译时确定，不能在运行时更改
// 如果需要运行时配置，需要自己实现

class RuntimeNewline {
public:
    enum class Style { CRLF, LF, CR };

    static const char* getSeparator(Style style) {
        switch (style) {
        case Style::CRLF: return "\r\n";
        case Style::LF: return "\n";
        case Style::CR: return "\r";
        default: return "\r\n";
        }
    }
};
```

### 2. 与 printf 的交互

```cpp
// 使用 EMDEVIF_LINE_SEPARATOR 作为格式字符串的一部分（以 CRLF 为例）
printf("Value: %d" EMDEVIF_LINE_SEPARATOR, 42);
// 等价于 printf("Value: %d\r\n", 42);

printf("Value:" EMDEVIF_LINE_SEPARATOR " %d", 42);
// 等价于 printf("Value:\r\n %d", 42);
```

### 3. 多行字符串

```cpp
// 多行字符串中的换行符不受 EMDEVIF_LINE_SEPARATOR 影响
const char* multi_line = "Line 1\nLine 2\nLine 3";
// 这里的 \n 是硬编码的，不会被替换

// 如果需要使用配置的换行符，需要手动构建
const char* line1 = "Line 1" EMDEVIF_LINE_SEPARATOR;
const char* line2 = "Line 2" EMDEVIF_LINE_SEPARATOR;
```

## 使用示例

### 命令行接口

```cpp
#include <stdio.h>
#include "emdevif/core/line_separator.h"

void printHelp() {
    printf("Available commands:" EMDEVIF_LINE_SEPARATOR);
    printf("  help    - Show this help" EMDEVIF_LINE_SEPARATOR);
    printf("  status  - Show system status" EMDEVIF_LINE_SEPARATOR);
    printf("  reset   - Reset the system" EMDEVIF_LINE_SEPARATOR);
}

void processCommand(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        printHelp();
    } else if (strcmp(cmd, "status") == 0) {
        printf("System OK" EMDEVIF_LINE_SEPARATOR);
    } else {
        printf("Unknown command: %s" EMDEVIF_LINE_SEPARATOR, cmd);
    }
}
```

### 状态报告

```cpp
#include <stdio.h>
#include "emdevif/core/line_separator.h"

void reportStatus(int uptime, int free_memory, int tasks) {
    printf("=== System Status ===" EMDEVIF_LINE_SEPARATOR);
    printf("Uptime: %d seconds" EMDEVIF_LINE_SEPARATOR, uptime);
    printf("Free Memory: %d bytes" EMDEVIF_LINE_SEPARATOR, free_memory);
    printf("Running Tasks: %d" EMDEVIF_LINE_SEPARATOR, tasks);
    printf("====================" EMDEVIF_LINE_SEPARATOR);
}
```

### 错误报告

```cpp
#include <stdio.h>
#include "emdevif/core/line_separator.h"

void reportError(const char* file, int line, const char* msg) {
    printf("ERROR at %s:%d: %s" EMDEVIF_LINE_SEPARATOR, file, line, msg);
}

void reportFatalError(const char* msg) {
    printf("FATAL ERROR: %s" EMDEVIF_LINE_SEPARATOR, msg);
    printf("System halted." EMDEVIF_LINE_SEPARATOR);
}
```

## 总结

换行符模块为嵌入式系统提供了简单、高效的换行符配置机制：

- **编译时配置**：零运行时开销
- **统一接口**：隐藏不同平台的换行符差异
- **易于使用**：简单的字符串字面量拼接

理解这个模块的设计理念和使用场景，可以帮助编写出更可移植、更易配置的嵌入式代码。