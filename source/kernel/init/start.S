#include "os_cfg.h"

    .text
    .global _start
    .extern kernel_init
    .extern init_main
    # void start (boot_info_t * boot_info)
_start:
    // 取参!!!
    mov 0x4(%esp), %eax
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

    .text
    .extern do_handler_timer
    .global exception_handler_timer
    # cs eip code_error EFLAGS等由CPU保存
exception_handler_timer:
    # 栈此时布局
    # eflags cs ip code_error   (CPU)
    # eax ecx edx ebx esp ebp esi edi  (pusha)
    # ds es fs gs       (push)
    # return_address    (call)
    # .if \with_err_code == 0
    #     push $0 # 错误码
    # .endif

    # push $\num # 向量号
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    push %esp
    call do_handler_timer
    add $(1*4), %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    # add $(2*4), %esp
    iret

exception_handler unknown, -1, 0
exception_handler divider, 0, 0
exception_handler Debug, 1, 0
exception_handler NMI, 2, 0
exception_handler breakpoint, 3, 0
exception_handler overflow, 4, 0
exception_handler bound_range, 5, 0
exception_handler invalid_opcode, 6, 0
exception_handler device_unavailable, 7, 0
exception_handler double_fault, 8, 1
exception_handler invalid_tss, 10, 1
exception_handler segment_not_present, 11, 1
exception_handler stack_segment_fault, 12, 1
exception_handler general_protection, 13, 1
exception_handler page_fault, 14, 1
exception_handler fpu_error, 16, 0
exception_handler alignment_check, 17, 1
exception_handler machine_check, 18, 0
exception_handler smd_exception, 19, 0
exception_handler virtual_exception, 20, 0
# exception_handler timer, 0x20, 0

    .text
    .global simple_switch
    // simple_switch(&from, to)
simple_switch:
    // 4(%esp) 是当前任务结构中stack处的地址
    mov 4(%esp), %eax  // 取from->stack
    // 取出to任务的stack栈顶指针
    mov 8(%esp), %edx  // 取to-stack

    // 在当前任务栈中保存上下文
    push %ebp
    push %ebx
    push %esi
    push %edi
    // 将当前任务的esp保存入TSS结构体中
    mov %esp, (%eax)  // from-stack = esp
    // 切换为to任务的esp
    mov %edx, %esp    // esp = to->stack

    // to任务栈中恢复上下文
    pop %edi
    pop %esi
    pop %ebx
    pop %ebp

    ret