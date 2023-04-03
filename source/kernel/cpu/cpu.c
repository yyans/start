#include "cpu/cpu.h"
#include "os_cfg.h"
#include "comm/cpu_instr.h"

static segment_desc_t gdt_table[GDT_TABLE_SIZE];

// 设置gdt表
void segmen_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr) {
    segment_desc_t *desc = gdt_table + (selector >> 3);

    if (limit > 0xFFFFF) {
        attr != 0x8000;
        limit /= 0x1000;
    }

    desc->limit15_0 = limit & 0xFFFF;
    desc->base15_0 = base & 0xFFFF;
    desc->base23_16 = (base >> 16) & 0xFF;
    desc->attr = attr | (((limit >> 16) & 0xF) << 8);
    desc->base31_24 = (base >> 24) & 0xFF;
}

void gate_desc_set(gate_desc_t *desc, uint16_t selector, uint32_t offset, uint16_t attr) {
    desc->offset15_0 = offset & 0xFFFF;
    desc->selector = selector;
    desc->attr = attr;
    desc->offset31_16 = (offset >> 16) & 0xFFFF;
}

// 初始化gdt表
void init_gdt(void) {
    for (int i = 0; i < GDT_TABLE_SIZE; i++) {
        // i << 3 起始于表头的偏移量
        segmen_desc_set(i << 3, 0, 0, 0);
    }

    // 设置表属性
    segmen_desc_set(KERNEL_SELECTOR_CS, 0, 0xFFFFFFFF, 
        SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D);
    segmen_desc_set(KERNEL_SELECTOR_DS, 0, 0xFFFFFFFF, 
        SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D);

    // 重新加载gdt表  
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}

void cpu_init(void) {
    init_gdt();
}