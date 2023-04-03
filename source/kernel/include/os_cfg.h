#ifndef OS_CFG_H
#define OS_CFG_H

#define GDT_TABLE_SIZE 8192

#define KERNEL_SELECTOR_CS (1 << 3)
#define KERNEL_SELECTOR_DS (2 << 3)
#define KERNEL_STACK_SIZE  (8 * 1024)

#endif