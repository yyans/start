#ifndef CPU_INSTR_H
#define CPU_INSTR_H

#include "types.h"

// 声明在头文件中 因此可能被多个文件调用
// 编译器此时不一定会按照inline的方式编译 很可能当成函数编译
// 其他c文件include此头文件 因此函数出现在不同文件中需要加static

// 关中断
static inline void cli(void) {
    __asm__ __volatile__("cli");
}

// 开中断
static inline void sti(void) {
    __asm__ __volatile__("sti");
}

// 从port端口读入一个8位的数据
static inline uint8_t inb(uint16_t port) {
    uint8_t rv;
    
    // inb al, dx
    __asm__ __volatile__("inb %[p], %[v]"
        : [v]"=a"(rv) 
        : [p]"d"(port)
    );
    return rv;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t rv;
    
    // inb ax, dx
    __asm__ __volatile__("in %[p], %[v]"
        : [v]"=a"(rv) 
        : [p]"d"(port)
    );
    return rv;
}

// 向port端口写入一个8位数据
static inline uint8_t outb(uint16_t port, uint8_t data) {
    // outb al, dx
    __asm__ __volatile__("outb %[v], %[p]"::[p]"d"(port), [v]"a"(data));
}

// 加载gdt
static inline void lgdt(uint32_t start, uint32_t size) {
	struct {
		uint16_t limit;
		uint16_t start15_0;
		uint16_t start31_16;
	} gdt;

	gdt.start31_16 = start >> 16;
	gdt.start15_0 = start & 0xFFFF;
	gdt.limit = size - 1;

	__asm__ __volatile__("lgdt %[g]"::[g]"m"(gdt));
}

// 加载idt表
static inline void lidt(uint32_t start, uint32_t size) {
	struct {
		uint16_t limit;
		uint16_t start15_0;
		uint16_t start31_16;
	} idt;

	idt.start31_16 = start >> 16;
	idt.start15_0 = start & 0xFFFF;
	idt.limit = size - 1;

	__asm__ __volatile__("lidt %0"::"m"(idt));
}

static inline uint32_t read_cr0() {
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %[v]":[v]"=r"(cr0));
	return cr0;
}

static inline void write_cr0(uint32_t v) {
	__asm__ __volatile__("mov %[v], %%cr0"::[v]"r"(v));
}

static inline void far_jump(uint32_t selector, uint32_t offset) {
	uint32_t addr[] = {offset, selector };
	__asm__ __volatile__("ljmpl *(%[a])"::[a]"r"(addr));
}

#endif