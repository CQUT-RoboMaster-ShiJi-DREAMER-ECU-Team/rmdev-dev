# emdevif_timeline

微秒级时间戳以及间隔时间测量。

## 配置

* 宏 `EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS`（CMake 变量 `EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS`）：如果为 ON，则 `Timeline` 最高精度为毫秒。默认为 OFF。

## 链接期注入的需求

该库通过链接期注入机制获取时间源。用户只需在任意一个自己的 `.cpp` 文件中，于命名空间 `emdevif::user_impl::timeline` 内定义对应函数即可：

```C++
#include <cstdint>  // for uint64_t

namespace emdevif::user_impl::timeline {

// 用于获取以微秒为单位的时间戳（EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS 为 OFF 时）
uint64_t getMicroseconds() noexcept
{
    // 返回以微秒为单位的时间戳
}

// 如果 EMDEVIF_TIMELINE_SOURCE_IS_MILLISECONDS 为 ON，则改为提供 getMilliseconds 函数，返回以毫秒为单位的时间戳
// uint64_t getMilliseconds() noexcept
// {
//     // 返回以毫秒为单位的时间戳
// }

}  // namespace emdevif::user_impl::timeline
```

注意：该实现文件需要链接到最终的可执行文件或库，链接器会自动解析符号。

## 示例

```C++
#include <iostream>
import emdevif.timeline;

int main()
{
    using namespace emdevif;

    Timeline tl;
    std::cout << tl() << "\n";  // 由于 tl 未初始化，因此输出的值未知

    tl.update();  // 更新 tl 存储的时间戳
    std::cout << tl() << "\n";  // 输出执行上一行时的时间戳（以微秒为单位）

    Timeline::pauseDelayMs(5);  // 阻塞式延时 5 毫秒（用空循环实现）

    Duration duration;
    duration.update();  // 更新间隔时间
    while (true) {
        // ...

        auto d1 = duration.getMicroDuration();
        // 获取间隔时间（从上次调用 duration.update() 或 duration.getMicroDuration() 开始计时）

        auto d2 = duration.getSecondsDuration();
        // 获取间隔时间（以秒为单位。如果 Duration 的模板参不是浮点类型，有可能损失精度）
    }
}
```
