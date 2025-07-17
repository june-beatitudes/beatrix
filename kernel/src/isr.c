#include <faults.h>
#include <isr.h>
#include <stddef.h>
#include <syscall.h>

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
        NULL,
        bea_do_syscall };
