#include "init.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "tools/log.h"
#include "os_cfg.h"


void kernel_init(boot_info_t *boot_info) {
    cpu_init();

    // log_init();
    irq_init();
    time_init();
}

void init_main() {
    // log_printf("welcome to my kernel!");
    // log_printf("Version: %s", OS_VERSION);
    irq_enable_global();
    for(;;) {}
}