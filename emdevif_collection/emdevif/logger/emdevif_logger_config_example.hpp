#pragma once
#ifndef EMDEVIF_LOGGER_USER_CONFIG_HPP
#define EMDEVIF_LOGGER_USER_CONFIG_HPP

// 日志忽略等级：如果日志等级小于该值，则不会打印该日志（如果日志等级与忽略等级相等，则仍然会打印该日志）。
// 取值可选：
//     0 - Verbose
//     1 - Debug
//     2 - Info
//     3 - Warning
//     4 - Error
//     5 - Fatal
// 默认值为 2
#define EMDEVIF_LOGGER_IGNORE_LEVEL            2

// 日志缓冲区大小，单位为字节。仅在同步模式和异步模式下有效（EMDEVIF_LOGGER_MODE 为 0 或 1 时）
// 默认值为 256。
#define EMDEVIF_LOGGER_BUFFER_SIZE             256

// 日志缓冲区数量。仅在同步模式和异步模式下有效（EMDEVIF_LOGGER_MODE 为 0 或 1 时）
// 默认值为 4
#define EMDEVIF_LOGGER_BUFFER_COUNT            4

// 同步模式下，是否使用互斥锁，仅在同步模式下有效（EMDEVIF_LOGGER_MODE 为 0 时）
// 默认值为 true
#define EMDEVIF_LOGGER_SYNC_USE_LOCK           true

// 异步模式下，日志打印线程的栈深度，单位取决于线程的实现。
// 仅在异步模式下有效（EMDEVIF_LOGGER_MODE 为 1 时）。
// 默认值为 128
#define EMDEVIF_LOGGER_ASYNC_THREAD_STACK_SIZE 128

// 是否动态创建日志的对象（如线程、互斥锁等）。
// 静态创建的日志不能销毁。
// 默认值为 false
#define EMDEVIF_LOGGER_DYNAMIC_CREATE          false

#endif  // !EMDEVIF_LOGGER_USER_CONFIG_HPP
