
#include "loader.h"

extern boot_info_t boot_info;

// 使用LBA18位读取磁盘
static void read_disk(uint32_t sector, uint32_t sector_count, uint16_t *buf) {
    outb(0x1F6, (uint8_t) (0xE0));

	outb(0x1F2, (uint8_t) (sector_count >> 8));
    outb(0x1F3, (uint8_t) (sector >> 24));		// LBA参数的24~31位
    outb(0x1F4, (uint8_t) (0));					// LBA参数的32~39位
    outb(0x1F5, (uint8_t) (0));					// LBA参数的40~47位

    outb(0x1F2, (uint8_t) (sector_count));
	outb(0x1F3, (uint8_t) (sector));			// LBA参数的0~7位
	outb(0x1F4, (uint8_t) (sector >> 8));		// LBA参数的8~15位
	outb(0x1F5, (uint8_t) (sector >> 16));		// LBA参数的16~23位

	outb(0x1F7, (uint8_t) 0x24);

	// 读取数据
	uint16_t *data_buf = (uint16_t*) buf;
	while (sector_count-- > 0) {
		// 每次扇区读之前都要检查，等待数据就绪
		while ((inb(0x1F7) & 0x88) != 0x8) {}

		// 读取并将数据写入到缓存中
		for (int i = 0; i < SECTOR_SIZE / 2; i++) {
			*data_buf++ = inw(0x1F0);
		}
	}
}

/**
 * z栈的作用:
 * 1. 保存局部变量和数据
 * 2. 传递参数: 从参数列表右侧往左压栈
 * 3. 保存返回地址
 * 4. 通过ebp+偏移取调用者的传入的参数
 */

static uint32_t reload_elf_file(uint8_t *file_buffer) {
    Elf32_Ehdr *elf_hdr = (Elf32_Ehdr *)file_buffer;

    // 检查ELF文件是否合法
    if ((elf_hdr->e_ident[0] != 0x7F) || (elf_hdr->e_ident[1] != 'E')
        || (elf_hdr->e_ident[2] != 'L') || (elf_hdr->e_ident[3] != 'F')) {
            return 0;
    }
    
    // 根据相应字段值加载对应program header
    for (int i = 0; i < elf_hdr->e_phnum; i++) {
        Elf32_Phdr *phdr = (Elf32_Phdr *)(file_buffer + elf_hdr->e_phoff) + i;
        if (phdr->p_type != PT_LOAD) {
            continue;
        }
        
        // ELF文件在内存中的起始地址加上相应的偏移地址 得到对应代码段的地址
        uint8_t *src = file_buffer + phdr->p_offset;
        // 将段加载到内存中的相应位置
        uint8_t *dest = (uint8_t *)phdr->p_paddr;
        // 一个一个字节的读取
        for (int j = 0; j < phdr->p_filesz; j++) {
            *dest++ = *src++;
        }
        // 在data段中会存放一些未初始化的全局变量 这些变量会被赋值为0
        // 但是在data段中不必要存放0（在ELF中没有分配相应的空间）
        // 需要我们自己在内存中清0
        // 完成bss区的初始化
        dest = (uint8_t *)phdr->p_paddr + phdr->p_filesz;
        for (int j = 0; j < phdr->p_memsz - phdr->p_filesz; j++) {
            *dest++ = 0;
        }
        
    }

    return elf_hdr->e_entry;
}

static void die(int code) {
    for (;;) {}
}

void load_kernel(void) {

    // 读取kernel.elf到0x100000
    read_disk(100, 500, (uint16_t *)SYS_KERNEL_LOAD_ADDR);

    // 对内核的ELF进行解析
    // 获取text代码段的入口地址
    uint32_t kernel_entry = reload_elf_file((uint8_t *)SYS_KERNEL_LOAD_ADDR);
    if (kernel_entry == 0) {
        die(-1);
    }

    ((void (*)(boot_info_t *))kernel_entry)(&boot_info);

    for (;;) {}
}