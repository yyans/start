#include "init.h"
#include "comm/boot_info.h"
#include "comm/cpu_instr.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "tools/kilb.h"
#include "core/task.h"
#include "tools/list.h"

static char init_task_stack[1024];

void kernel_init(boot_info_t *boot_info) {
    ASSERT(boot_info->ram_region_count != 0);

    cpu_init();

    log_init();
    irq_init();
    time_init();
}

static task_t init_task;
static task_t first_task;
void init_task_entry (void) {
    int count = 0;
    for (;;) {
        log_printf("init task: %d", count++);
        task_switch_from_to(&init_task, &first_task);
    }
}

void list_test (void) {
    list_t list;
    list_node_t nodes[5];

    list_init(&list);
    log_printf("f: %x, last: %x, count: %d", list_first(&list), list_last(&list), list_count(&list));

    for (int i = 0; i < 5; i++) {
        list_node_t * node = nodes + i;
        log_printf("insert : first to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_first(&list, node);
    }

    log_printf("f: %x, last: %x, count: %d", 
        list_first(&list), list_last(&list), list_count(&list));

    list_init(&list);
    for (int i = 0; i < 5; i++) {
        list_node_t * node = nodes + i;
        log_printf("insert : first to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_last(&list, node);
    }

    log_printf("f: %x, last: %x, count: %d", 
        list_first(&list), list_last(&list), list_count(&list));

    // remove first
    for (int i = 0; i < 5; i++) {
        list_node_t * node = list_remove_first(&list);
        log_printf("remove : first to list: %d, 0x%x", i, (uint32_t)node);
    }

    log_printf("f: %x, last: %x, count: %d", 
        list_first(&list), list_last(&list), list_count(&list));
    
    // remove
    list_init(&list);
    for (int i = 0; i < 5; i++) {
        list_node_t * node = nodes + i;
        log_printf("insert : first to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_last(&list, node);
    }

    log_printf("f: %x, last: %x, count: %d", 
        list_first(&list), list_last(&list), list_count(&list));

    for (int i = 0; i < 5; i++) {
        list_node_t * node = nodes + i;
        log_printf("remove : first to list: %d, 0x%x", i, (uint32_t)node);
        list_remove(&list, node);
    }

    log_printf("f: %x, last: %x, count: %d", 
        list_first(&list), list_last(&list), list_count(&list));

    struct t {
        int i;
        list_node_t node;
    }v = {0x123456};

    list_node_t * v_node = &v.node;
    struct t * p = list_node_parent(v_node, struct t, node);
    if (p->i != 0x123456) {
        log_printf("error");
    }
}

void init_main() {
    log_printf("welcome to my kernel!");
    log_printf("Version: %s %s", OS_VERSION, "kzj os");
    log_printf("%d %d %x %c", 1234567, -123, 0x12345, 'a');

    // 初始化任务
    task_init(&init_task, (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);
    task_init(&first_task, 0, 0);
    write_tr(first_task.tss_sel);

    list_test();
    int count = 0;
    for (;;) {
        log_printf("int main: %d", count++);
        task_switch_from_to(&first_task, &init_task);
    }

    init_task_entry();
}