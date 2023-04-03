/**
 * 自己动手写操作系统
 *
 * 系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * 该部分程序存储于磁盘的第1个扇区，在计算机启动时将会由BIOS加载到0x7c00处
 * 之后，将由BIOS跳转至0x7c00处开始运行
 * 
 * 作者：李述铜
 * 联系邮箱: 527676163@qq.com
 */

  	// 16位代码，务必加上
  	.code16
 	.text
	.global _start
	// 声明一下这个函数在其他文件种定义
	.extern boot_entry
_start:
	// 初始化各段寄存器 平坦模型
	mov $0, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	// 初始化boot栈顶地址
	mov $_start, %esp

	// 调用中断服务显示字符
	mov $0xe, %ah
	mov $'L', %al
	int $0x10

read_loader:
	mov $0x8000, %bx
	mov $0x2, %ah
	mov $0x2, %cx
	// 读取扇区尽量大一些
	mov $64, %al
	mov $0x0080, %dx
	int $0x13
	// CF0成功
	// 读取失败就反复读
	jc read_loader

	jmp boot_entry
	jmp .
	
	// 引导结束段
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa
