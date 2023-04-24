#include "core/task.h"
#include "tools/kilb.h"
#include "os_cfg.h"
#include "cpu/cpu.h"
#include "tools/log.h"
#include "comm/cpu_instr.h"

static task_manager_t task_manager;

static int tss_init(task_t *task, uint32_t entry, uint32_t esp) {
    int tss_sel = gdt_alloc_desc();
    if (tss_sel < 0) {
        log_printf("alloc tss failed.");
        return -1;
    }

    // 设置GDT表中的TSS描述符
    segmen_desc_set(tss_sel, (uint32_t)&task->tss, sizeof(tss_t),
        SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS
    );

    kernel_memset(&task->tss, 0, sizeof(tss_t));
    task->tss.eip = entry;
    task->tss.esp = task->tss.esp0 = esp;
    task->tss.ss0 = KERNEL_SELECTOR_DS;
    task->tss.eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    task->tss.es = task->tss.ss = task->tss.ds
            = task->tss.fs = task->tss.gs = KERNEL_SELECTOR_DS;   // 暂时写死
    task->tss.cs = KERNEL_SELECTOR_CS;    // 暂时写死
    task->tss.iomap = 0;

    task->tss_sel = tss_sel;
    return 0;
}

// 初始化一个任务
// 初始化 tss
// 初始化 name
// 将任务节点都加入对应队列 同时根据加入队列的不同设置不同状态
int task_init (task_t *task, char * name, uint32_t entry, uint32_t esp) {
    ASSERT(task != (task_t *)0);

    tss_init(task, entry, esp);

    kernel_strncpy(task->name, name, TASK_NAME_SIZE);
    task->state = TASK_CREATED;
    list_node_init(&task->all_node);
    list_node_init(&task->run_node);

    task_set_ready(task); // !!
    list_insert_last(&task_manager.task_list, &task->all_node);

    return 0;
}

void simple_switch (uint32_t **from, uint32_t *to);

// 从 from 任务切换到 to 任务
void task_switch_from_to (task_t * from, task_t * to) {
    switch_to_tss(to->tss_sel);
    // simple_switch(&from->stack, to->stack);
}

void task_first_init (void) {
    task_init(&task_manager.first_task, "first task", 0, 0);
    write_tr(task_manager.first_task.tss_sel);
    task_manager.curr_task = &task_manager.first_task;
}

task_t * task_first_task (void) {
    return &task_manager.first_task;
}

// 初始化任务管理器
void task_mananger_init(void) {
    // 初始化就绪队列
    list_init(&task_manager.ready_list);
    // 初始化所有任务队列
    list_init(&task_manager.task_list);
    task_manager.curr_task = (task_t *)0;
}

// 设置任务为就绪状态 并且加入就绪队列 队尾 中
void task_set_ready(task_t * task) {
    list_insert_last(&task_manager.ready_list, &task->run_node);
    task->state = TASK_RUNNING;
}

// 将任务从就绪队列中移除
void task_set_block (task_t * task) {
    list_remove(&task_manager.ready_list, &task->run_node);
}

// 获取接下来要运行的任务
// 获取当前链表的队首 用之前的宏即可获取这个节点所在结构体的起始地址
task_t * task_next_run (void) {
    list_node_t * task_node = list_first(&task_manager.ready_list);
    
    return list_node_parent(task_node, task_t, run_node);
}

// 获取当前正在执行任务的指针
task_t * task_currnet (void) {
    return task_manager.curr_task;
}

// 切换任务
int sys_sched_yield (void) {
    if (list_count(&task_manager.ready_list) > 1) {
        task_t * curr_task = task_currnet();

        task_set_block(curr_task);
        task_set_ready(curr_task);

        task_dispatch();
    }

    return 0;
}

// 任务分配
// from -> to
void task_dispatch (void) {
    task_t * to = task_next_run();
    if (to != task_manager.curr_task) {
        task_t * from = task_currnet();

        task_manager.curr_task = to;
        to->state = TASK_RUNNING;

        task_switch_from_to(from, to);
    }
}