#include "os_cfg.h"

    .text
    .global _start
    .extern kernel_init
    .extern init_main
    # void start (boot_info_t * boot_info)
_start:
    // 取参!!!
    push %ebp
    mov %esp, %ebp
    mov 0x8(%ebp), %eax
    push %eax

    # kernel_init(boot_info)
    call kernel_init
    
    
    jmp $KERNEL_SELECTOR_CS, $gdt_reload
gdt_reload:
    mov $KERNEL_SELECTOR_DS, %ax

    mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    mov $(stack + KERNEL_STACK_SIZE), %esp
    jmp init_main
    

    .bss
.comm stack, KERNEL_STACK_SIZE

    .text
    .extern do_handler_unknown
    .global exception_handler_unknown
    # cs eip code_error EFLAGS等由CPU保存
exception_handler_unknown:
    # 栈此时布局
    # eflags cs ip code_error   (CPU)
    # eax ecx edx ebx esp ebp esi edi  (pusha)
    # ds es fs gs       (push)
    # return_address    (call)
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    push %esp
    call do_handler_unknown
    add $(1*4), %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    iret