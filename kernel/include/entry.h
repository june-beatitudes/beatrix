#ifndef BEA_KERN_ENTRY_H
#define BEA_KERN_ENTRY_H

#define STACK_TOP (void *)(0x20030000)

#include "common.h"

void bea_kern_onreset (void);

typedef void (*ivt_entry_t) (void);

__attribute__ ((section (".ivt"))) const static ivt_entry_t IVT[] = {
        (ivt_entry_t)STACK_TOP,
        bea_kern_onreset,
};

#endif