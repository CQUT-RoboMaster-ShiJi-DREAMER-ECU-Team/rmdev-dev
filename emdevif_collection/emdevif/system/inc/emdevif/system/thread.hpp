/**
 * @file thread.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_SYSTEM_THREAD_HPP
#define EMDEVIF_SYSTEM_THREAD_HPP

#include <concepts>
#include <functional>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include "emdevif/core/attributes_and_useful_macros.h"
#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/concepts.hpp"
#include "emdevif/core/error_handler.hpp"
#include "emdevif/system/heap.hpp"

namespace emdevif::detail {
class ThreadTypeChecker;  // 前向声明
}

namespace emdevif {

#ifndef EMDEVIF_SYS_TICK_TYPE_UINT_BITS
#define EMDEVIF_SYS_TICK_TYPE_UINT_BITS 64
#endif

#if (EMDEVIF_SYS_TICK_TYPE_UINT_BITS == 16)
using SysTick_t = uint16_t;
#elif (EMDEVIF_SYS_TICK_TYPE_UINT_BITS == 32)
using SysTick_t = uint32_t;
#elif (EMDEVIF_SYS_TICK_TYPE_UINT_BITS == 64)
using SysTick_t = uint64_t;
#else
#error "The value of macro `EMDEVIF_SYS_TICK_TYPE_UINT_BITS' can only equals to 16, 32 or 64."
#endif

/**
 * 线程标准入口函数指针
 */
using ThreadEntry = void (*)(void*);

/**
 * 优先级
 */
enum class ThreadPriority : int_fast8_t {
    Idle = -1,    ///< 空闲任务优先级（确保与空闲优先级一致）

    Low = 0,      ///< 低优先级
    BelowNormal,  ///< 低于常规优先级
    Normal,       ///< 常规优先级（默认值）
    AboveNormal,  ///< 高于常规优先级
    High,         ///< 高优先级
    Realtime,     ///< 实时优先级

    Max = -2      ///< 最高优先级（确保与最大优先级一致）
};

/**
 * @page sys_static_instance 静态实例
 * 静态实例是用于线程、信号量、系统队列等功能静态创建实例的载体，用于提供静态创建的内存块。
 *
 * 在接口中，静态实例仅作声明，定义在实现模块分区中（因为不同的操作系统的静态创建所需要的类型不一定相同）
 */

/**
 * 线程的静态实例
 *
 * @copydoc sys_static_instance
 *
 * 对于开发者：线程的静态实例需要提供 `getInstanceAddr` 和 `getStackDepth` 方法，用于用户创建静态实例时调用。
 * @tparam stack_depth 栈的深度（以字为单位）
 */
template<std::size_t stack_depth>
class ThreadStaticInstance;

/**
 * 线程 Builder，用于 Thread::create 的参数。
 */
struct ThreadBuilder {
    const char* name{};                               ///< 名称。必填参数。
    ThreadPriority priority{ThreadPriority::Normal};  ///< 初始的优先级（默认为常规优先级）

    void* static_instance{nullptr};                   ///< 静态实例内存
    std::size_t stack_size{0U};                       ///< 栈深度
};

/**
 * 线程类
 */
class Thread
{
public:
    using Handle = void*;  ///< 线程句柄

    friend class emdevif::detail::ThreadTypeChecker;

    /**
     * 优先级映射范围。
     *
     * 由于 ThreadPriority 只有 8 个等级，而实际的系统可能有更多的优先级，因此需要把 ThreadPriority
     * 中的等级映射到实际系统的优先级中。这个映射的方式是：保持靠左居中。例如实际系统有 9
     * 个优先级，从 0 ~ 8 分别对应优先级最低到最高，那么 ThreadPriority 的 Low ~ Realtime
     * 将会映射到实际优先级的 1 ~ 6，ThreadPriority::Idle 与 ThreadPriority::Max 则分别对应 0 和 8。
     * @return std::pair 类型，值分别是 ThreadPriority::Low 与 ThreadPriority::Realtime
     *         映射到实际系统的优先级。
     */
    static consteval auto priorityMapRange() noexcept;

private:
    /**
     * 将给定的 ThreadPriority 映射到实际系统的优先级
     * @param priority 优先级
     * @return 这个优先级映射到实际系统的优先级的值
     */
    static constexpr auto priorityMapToSystem(ThreadPriority priority) noexcept;

public:
    enum class MulParam {
        mulparam
    };
    /// 多参数线程入口创建标志
    static constexpr auto mulparam = MulParam::mulparam;

    /**
     * 等待时间最大值
     */
    static consteval SysTick_t maxDelay() noexcept;

    /**
     * 获取当前系统时间戳的时间值
     * @param in_isr 是否在中断中调用的这个函数
     * @return 当前系统时间戳的时间值
     */
    static SysTick_t getTick(bool in_isr) noexcept;

    /**
     * 延时
     * @param ticks 需要延时的 tick 数
     */
    static void delay(SysTick_t ticks) noexcept;

    /**
     * 绝对延时
     *
     * 示例：@code{.cpp}
     * import emdevif.sys.thread;
     * using namespace emdevif;
     *
     * constexpr auto delay_ticks = Thread::msToTick(100);
     * Thread::delayUntil(Thread::getTick(false) + delay_ticks);  // 不能在中断里使用
     * @endcode
     * @param ticks 要等待到的绝对时间
     * @return 如果成功延时，返回 Success。如果预计等待到的绝对时间已经过去，返回 InvalidArgument。
     */
    static ErrorCode delayUntil(SysTick_t ticks) noexcept;

    /**
     * 将毫秒时间转化成 tick 单位
     * @param ms 毫秒
     * @return 转化后的 tick 数
     */
    static SysTick_t msToTick(SysTick_t ms) noexcept;

    /**
     * 创建线程
     *
     * 动态创建的使用示例：@code{.cpp}
     * import emdevif.sys.thread;
     * using namespace emdevif;
     *
     * void threadEntry(void* arguments)
     * {
     *     auto& arg_ = *static_cast<int*>(arguments);  // arg_ 是传入的参数 arg 的引用
     *     // 使用时请注意 arg 的生命周期，避免悬垂引用。
     *
     *     // do something...
     * }
     *
     * int arg;
     *
     * void init()
     * {
     *     Thread thread;  // 此时，这个线程未创建
     *
     *     thread = Thread::create({.name = "thread"}, threadEntry, &arg);
     *     // 第一个参数不填写 .static_instance 和 .stack_size 即为动态创建
     *
     *     // thread 离开作用域后会自动删除，因此您可以将它定义在全局区域内
     * }
     * @endcode
     *
     * 静态创建的使用示例：@code{.cpp}
     * import emdevif.sys.thread;
     * using namespace emdevif;
     *
     * constexpr auto thread_instance_stack_size = 128;
     * static ThreadStaticInstance<thread_instance_stack_size> thread_instance;
     * Thread thread;  // 静态创建的线程不允许删除，因此它必须在全局区定义或者加上 static
     *
     * void init()
     * {
     *     thread = Thread::create({.name = "thread",
     *                              .static_instance = thread_instance.getInstanceAddr(),
     *                              .stack_size = thread_instance.getStackDepth()
     *                             },
     *                             threadEntry,
     *                             &arg);
     * }
     * @endcode
     * @param builder Builder，包含名称、优先级、静态实例、栈深度等属性
     * @param entry 线程标准入口，即一个签名为 void(void*) 的函数
     * @param arguments 传入线程标准入口的参数
     * @return 创建的线程实例
     */
    static Thread create(const ThreadBuilder& builder, ThreadEntry entry, void* arguments) noexcept;

private:
    template<typename Tuple, std::size_t... Indexes>
    static void mulParamEntryInvoker_(void* args) noexcept
    {
        const heap::unique_ptr<Tuple> func_invoke_data{static_cast<Tuple*>(args)};
        Tuple& tuple = *func_invoke_data.get();

        std::invoke(std::move(std::get<Indexes>(tuple))...);
    }

    template<typename Tuple, std::size_t... Indexes>
    static constexpr auto getMulParamEntryInvoker_(std::index_sequence<Indexes...>) noexcept
    {
        return &mulParamEntryInvoker_<Tuple, Indexes...>;
    }

public:
    /**
     * 创建任意参数的入口函数的线程
     *
     * 使用示例：@code{.cpp}
     * #include <functional>  // std::ref requires
     *
     * import emdevif.sys.thread;
     * using namespace emdevif;
     *
     * Thread thread;
     *
     * void init()
     * {
     *     int capture;
     *     int ref_capture;
     *     int ref_value;
     *
     *     thread = Thread::create(
     *         {.name = "thread"},
     *         Thread::mulparam,  // 这个参数用于表示这个函数是任意类型的参数
     *         [capture, &ref_capture](int a, float f, int& ref) {  // 函数对象、有捕获列表的 lambda 表达式也可以
     *             // do something...
     *         },
     *         1,
     *         123.456f,
     *         std::ref(ref_value)
     *     );
     * }
     * @endcode
     * @attention 会使用系统的堆内存，因此需要启动调度器后才能创建（不能静态创建）。
     * @tparam Func 线程的入口函数的类型
     * @tparam Args 这个入口函数的参数包
     * @param builder Builder
     * @param Mulparam 必须传入 Thread::mulparam 以表示创建的线程的入口函数的参数类型、数量是任意的
     * @param entry 线程的入口函数（可以是函数对象。函数返回值必须是 void，参数类型任意）
     * @param args 入口函数的参数
     * @return 创建好的线程实例
     */
    template<typename Func, typename... Args>
        requires(!std::same_as<std::remove_cvref_t<Func>, Thread> && !std::same_as<std::remove_cvref_t<Func>, MulParam>)
    static Thread create(const ThreadBuilder& builder, MulParam, Func&& entry, Args&&... args) noexcept
    {
        // 参考教程：
        // https://mq-b.github.io/ModernCpp-ConcurrentProgramming-Tutorial/md/%E8%AF%A6%E7%BB%86%E5%88%86%E6%9E%90/01thread%E7%9A%84%E6%9E%84%E9%80%A0%E4%B8%8E%E6%BA%90%E7%A0%81%E8%A7%A3%E6%9E%90.html

        using TupleType = std::tuple<std::decay_t<Func>, std::decay_t<Args>...>;
        auto decay_copied_args =
            heap::make_unique<TupleType>(std::nothrow, std::forward<Func>(entry), std::forward<Args>(args)...);
        if (decay_copied_args.get() == nullptr) {
            return Thread{};
        }

        constexpr std::size_t tuple_element_count = 1U + sizeof...(Args);
        constexpr PointerType auto invoker =
            getMulParamEntryInvoker_<TupleType>(std::make_index_sequence<tuple_element_count>{});

        auto t = create(builder, invoker, decay_copied_args.get());
        if (t.handle_ != nullptr) {
            decay_copied_args.release();
        }
        return t;
    }

    /**
     * 删除线程
     * @attention 不能删除静态创建的线程
     * @param obj 待删除的线程实例
     * @return 成功删除返回 Success；传入未创建好的或者已删除的线程实例返回 InvalidArgument
     */
    static ErrorCode destroy(Thread& obj) noexcept;

    /**
     * 退出当前线程
     *
     * 使用示例：@code{.cpp}
     * import emdevif.sys.thread;
     * using namespace emdevif;
     *
     * Thread thread;
     *
     * void threadEntry(Thread& th)
     * {
     *     // do something...
     *
     *     th.exit();  // 退出当前线程
     *     // 注意：如果需要退出线程，不能直接退出，必须调用 th.exit() 才行。
     *
     *     // 程序将不会运行到此处
     * }
     *
     * // 创建线程的示例参考 Thread::create 方法
     * @endcode
     * @attention[1] 这个函数一旦调用就不会返回。
     * @attention[2] 静态创建的线程不能退出。
     */
    EMDEVIF_NO_RETURN void exit() noexcept;

    /**
     * 挂起线程
     * @param handle 待挂起的线程的句柄（可以通过 getHandle 方法获取）
     */
    static void suspend(Handle handle) noexcept;

    /**
     * 恢复被挂起的线程
     * @param in_isr 是否在中断里调用
     * @param handle 待恢复的线程的句柄（可以通过 getHandle 方法获取）
     */
    static void resume(bool in_isr, Handle handle) noexcept;

    /**
     * 启动调度器
     */
    static void startScheduler() noexcept;

    /**
     * 关闭调度器
     */
    static void endScheduler() noexcept;

    /**
     * 向实现提供一个提示，以重新调度当前线程的执行，允许其他线程运行。
     * @attention 此函数的具体行为取决于底层系统的实现，需要参考实际系统的文档。
     */
    static void yield() noexcept;

    /**
     * 检查当前对象是否可连接的
     * @return 可连接的状态
     */
    bool joinable() noexcept;

    /**
     * 连接线程。即阻塞当前线程，直到实例线程执行完毕
     */
    void join() noexcept;

    /**
     * 获得底层实现的句柄
     * @return 底层实现的句柄
     */
    [[nodiscard]] Handle getHandle() const noexcept
    {
        return handle_;
    }

    Thread() noexcept : handle_(nullptr) {}

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    Thread(Thread&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}

    Thread& operator=(Thread&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        if (handle_ != nullptr) {
            EMDEVIF_FATAL_HANDLER("Should not move-assign to a non-empty thread!");
            return *this;
        }

        this->handle_ = std::exchange(other.handle_, nullptr);
        return *this;
    }

    ~Thread() noexcept;

private:
    Handle handle_;  ///< 底层实现的句柄
};

}  // namespace emdevif

#include "emdevif/system_impl/thread.inl"

namespace emdevif::detail {

template<typename>
struct ValidThreadPriorityMapRangeReturnType : std::false_type {
};

template<std::unsigned_integral T, std::unsigned_integral U>
struct ValidThreadPriorityMapRangeReturnType<std::pair<T, U>> : std::true_type {
};

template<std::unsigned_integral T, std::unsigned_integral U>
struct ValidThreadPriorityMapRangeReturnType<std::tuple<T, U>> : std::true_type {
};

static_assert(ValidThreadPriorityMapRangeReturnType<std::remove_cvref_t<decltype(Thread::priorityMapRange())>>::value,
              "Error for `emdevif` developers, please report bug with the error message: The return type of "
              "`Thread::priorityMapRange()` must be `std::pair` or `std::tuple` with unsigned integral types.");

class ThreadTypeChecker
{
public:
    static_assert(
        std::is_unsigned_v<std::remove_cvref_t<decltype(Thread::priorityMapToSystem(ThreadPriority::Normal))>>,
        "Error for `emdevif` developers, please report bug with the error message: The return type of "
        "`Thread::priorityMapToSystem()` must be an unsigned integral type.");
};

}  // namespace emdevif::detail

#endif  // !EMDEVIF_SYSTEM_THREAD_HPP
