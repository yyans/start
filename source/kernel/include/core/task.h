#ifndef TASK_H
#define TASK_H

#include "comm/types.h"
#include "cpu/cpu.h"
#include "tools/list.h"

#define TASK_NAME_SIZE 32
#define TASK_TIME_SLICE_DEFAULT 10
// #define OS_TICKS_MS 100

typedef struct _task_t {
    // uint32_t * stack;
    enum {
        TASK_CREATED,
        TASK_RUNNING,
        TASK_SLEEP,
        TASK_READY,
        TASK_WATTING,
    }state;  // 任务状态

    int sleep_ticks;
    int time_ticks;
    int slice_ticks;
    
    char name[TASK_NAME_SIZE]; // 任务名称

    list_node_t run_node;      // 就绪节点 加入就绪队列和睡眠队列中
    list_node_t all_node;      // 加入所有队列中

    tss_t tss;                 // 任务的运行环境
    int tss_sel;               // GDT中对应TSS表项的索引
}task_t;

int task_init (task_t *task, char * name, uint32_t entry, uint32_t esp);
void task_switch_from_to (task_t * from, task_t * to);
void task_time_tick (void);

typedef struct _task_manager_t {
    task_t * curr_task;         // 指向当前任务

    list_t sleep_list;          // 睡眠队列
    list_t ready_list;          // 就绪队列
    list_t task_list;           // 所有任务队列

    task_t first_task;
    task_t idle_task;
} task_manager_t;

void task_mananger_init (void);
void task_first_init (void);
task_t * task_first_task (void);
void task_set_ready (task_t * task);
void task_set_block (task_t * task);
int sys_sched_yield (void);
task_t * task_currnet (void);
void task_dispatch (void);

void task_set_sleep (task_t * task, uint32_t ticks);
void task_set_wakeup (task_t * task);
void sys_sleep (uint32_t ms);


#endif