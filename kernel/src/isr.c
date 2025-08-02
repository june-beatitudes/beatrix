#include <faults.h>
#include <isr.h>
#include <stddef.h>
#include <syscall.h>
#include <task.h>

extern void bea_do_scheduler (void)  __asm__ ("bea_do_scheduler");

__attribute__ ((section (".isr_table"), unused)) isr_t const ISR_TABLE[]
    = { (isr_t)BEA_KERNEL_STACK_TOP,
        bea_do_reset,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        bea_do_syscall,
        NULL,
        NULL,
        bea_do_scheduler,
        bea_do_scheduler };
