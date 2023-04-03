#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "cpu/cpu.h"

#define IDE_TABLE_NR   128
static gate_desc_t idt_table[IDE_TABLE_NR];

void irq_init(void) {
    for (int i = 0; i < IDE_TABLE_NR; i++) {
        gate_desc_set(idt_table + i, 0, 0, 0);
    }

    lidt((uint32_t)idt_table, sizeof(idt_table));
}