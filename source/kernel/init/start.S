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

.macro exception_handler name num with_err_code
    .text
    .extern do_handler_\name
    .global exception_handler_\name
    # cs eip code_error EFLAGS等由CPU保存
exception_handler_\name:
    # 栈此时布局
    # eflags cs ip code_error   (CPU)
    # eax ecx edx ebx esp ebp esi edi  (pusha)
    # ds es fs gs       (push)
    # return_address    (call)
    .if \with_err_code == 0
        push $0 # 错误码
    .endif

    push $\num # 向量号
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    push %esp
    call do_handler_\name
    add $(1*4), %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2*4), %esp
    iret
.endm

exception_handler unknown, -1, 0
exception_handler divider, 0, 0
