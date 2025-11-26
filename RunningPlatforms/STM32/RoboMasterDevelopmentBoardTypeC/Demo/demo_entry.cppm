/**
 * @file demo_entry.cppm
 * @author DuYicheng
 * @date 2025-10-05
 * @brief 单元测试结束后的演示程序
 */

module;

#include <cstdint>

#include "emdevif/attributes_and_useful_macros.h"

#include "emdevif/fatal_handler.h"

export module afterUnitTestDemo;

import emdevif.errorHandler;
import emdevif.sys.thread;

import ins_task;

export EMDEVIF_NO_RETURN void demoEntry() noexcept
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
