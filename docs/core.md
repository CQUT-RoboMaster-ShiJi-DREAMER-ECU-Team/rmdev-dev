# emdevif_core

核心模块，所有库都需要它。

提供两个 CMake 目标：`emdevif_core` 和 `emdevif_compile_options`。前者是核心模块的静态库目标，
其他所有模块都需要链接它（以 PUBLIC 的方式）；后者是一个接口库目标，包含了一些编译选项（主要是配置编译警告），
其他 emdevif 的子模块也需要链接它（以 PRIVATE 的方式），而用户或第三方库的目标可以选择性地链接。

## 主要功能

### 概览

* 基本头文件（这些功能仅提供头文件，没有模块）：提供基础的数据类型定义和宏定义。
    * [attributes_and_useful_macros.h](../core/inc/emdevif/core/attributes_and_useful_macros.h): 提供常用的属性和宏定义。详见 [编译器属性与实用宏文档](core/attributes_and_useful_macros.md)。
    * [line_separator.h](../core/inc/emdevif/core/line_separator.h): 提供统一的换行符的定义。详见 [换行符模块文档](core/line_separator.md)。
    * [simplify_decl_macros.hpp](../core/inc/emdevif/core/simplify_decl_macros.hpp): 提供简化声明的宏。详见 [简化声明宏文档](core/simplify_decl_macros.md)。
  若头文件的后缀名是 `.h`，则表示它们在 C 和 C++ 中都可以使用；若后缀名是 `.hpp`，则表示它们只能在 C++ 中使用。
* [错误处理模块](../core/modules/error_handler.cppm): 提供错误码定义和错误处理函数。详见 [错误处理模块文档](core/error_handler.md)。
* [数据容器](../core/modules/data_container): 提供常用的数据容器实现（如环形缓冲区）。详见 [数据容器模块文档](core/data_container.md)。
* [integer_suffix.cppm](../core/modules/integer_suffix.cppm): 提供固定宽度整型、size_t、ptrdiff_t 的自定义字面量。详见 [整型后缀模块文档](core/integer_suffix.md)。
* STL 库的扩展：
    * [类型特征](../core/modules/type_traits.cppm): 提供部分类型特征的实现。详见 [类型特征模块文档](core/type_traits.md)。
    * [概念库](../core/modules/concepts.cppm): 提供一些常用概念的实现。详见 [概念库文档](core/concepts.md)。
    * [端序](../core/modules/endian.cppm): 提供端序转换相关的功能。详见 [端序转换模块文档](core/endian.md)。
    * [资源保护](../core/modules/resource_guard.cppm): 实现安全释放资源的机制（如 LockGuard、轻量级 try-finally）。详见 [资源保护模块文档](core/resource_guard.md)。
* 工具模块：提供通用化的控制结构和特殊类型。详见 [工具模块文档](core/utils.md)。

### CMake 的 emdevif_compile_options 接口库

这个库目前只有一个功能：编译时启用所有警告。emdevif 的所有子模块都会链接它（以 PRIVATE 的方式）。所有 emdevif 的内部实现都能够确保无警告。
但考虑到用户或第三方库的代码不一定能确保无警告，因此将开启所有警告的选项打包成接口库，用户可以自行决定是否链接它以开启所有警告。
