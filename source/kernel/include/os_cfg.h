#ifndef OS_CFG_H
#define OS_CFG_H

#define GDT_TABLE_SIZE 8192

#define KERNEL_SELECTOR_CS (1 << 3)
#define KERNEL_SELECTOR_DS (2 << 3)
#define KERNEL_STACK_SIZE  (8 * 1024)

#define OS_TICK_MS             10       	// 每毫秒的时钟数
#define IDLE_TASK_SIZE    1024

#define OS_VERSION              "1.0.1"

#endif