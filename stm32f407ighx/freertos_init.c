/**
 * @file freertos_init.c
 * @author DuYicheng
 * @date 2025-08-14
 * @brief FreeRTOS 任务初始化
 */

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "usart.h"

#include "emdevif/attributes_and_useful_macros.h"
#include "stm32_hal_test_func_impl.h"

void osStartDefaultTask(void* arguments);

#define DEFAULT_TASK_STACK_DEPTH 1024
TaskHandle_t default_task_handle = NULL;
StaticTask_t default_task_buffer;
StackType_t default_task_stack_buffer[DEFAULT_TASK_STACK_DEPTH];

void freeRtosInit(void)
{
    default_task_handle = xTaskCreateStatic(osStartDefaultTask,
                                            "Default Task",
                                            DEFAULT_TASK_STACK_DEPTH,
                                            NULL,
                                            configMAX_PRIORITIES - 1,
                                            default_task_stack_buffer,
                                            &default_task_buffer);
    configASSERT(default_task_handle != NULL);
}

EMDEVIF_NO_RETURN void osStartDefaultTask(void* arguments)
{
    EMDEVIF_UNUSED(arguments);

    testInit(&huart6);
    testEntry();

    while (true) {
        vTaskDelay(1);
    }
}
