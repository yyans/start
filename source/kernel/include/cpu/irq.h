#ifndef IRQ_H
#define IRQ_H

// 中断号
#define IRQ0_DE     0

typedef struct _exception_frame_t {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecs, eax;
    uint32_t num;
    uint32_t error_code;
    uint32_t eip, cs, eflags;
} exception_frame_t;

typedef void (*irq_handler_t)(void);

void irq_init(void);
int irq_install(int irq_num, irq_handler_t handler);
void exception_handler_unknown(void);
void exception_handler_divider(void);

#endif