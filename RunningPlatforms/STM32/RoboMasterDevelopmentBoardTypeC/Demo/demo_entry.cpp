/**
 * @file demo_entry.cpp
 * @brief 单元测试结束后的演示程序
 */

#include "demo_entry.hpp"

#include <cstdint>

#include "emdevif/core/attributes_and_useful_macros.h"
#include "emdevif/core/fatal_handler.h"

#include "emdevif/core/error_handler.hpp"
#include "emdevif/system/thread.hpp"

#include "INS_task.hpp"

EMDEVIF_NO_RETURN void demoEntry() noexcept
{
    using namespace emdevif;

    Thread ins_task = Thread::create({.name = "INS task", .priority = Thread::Priority::Realtime, .stack_size = 512},
                                     insTask,
                                     nullptr);
    if (ins_task.getHandle() == nullptr) {
        EMDEVIF_FATAL_HANDLER("Failed to create `INS task\'");
    }

    while (true) {
        Thread::delay(Thread::maxDelay());
    }
}
