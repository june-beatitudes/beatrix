#include <common.h>
#include <isr.h>

static inline int __attribute__ ((always_inline))
_bea_semihost_rq (int reason, void *arg)
{
  int value;
  __asm__ volatile ("MOV R0, %[rsn] \n"
                    "MOV R1, %[arg] \n"
                    "BKPT 0xAB \n"
                    "MOV %[val], R0 \n"
                    : [val] "=r"(value)
                    : [rsn] "r"(reason), [arg] "r"(arg)
                    : "r0", "r1", "r2", "r3", "ip", "lr", "memory", "cc");
  return value;
}

#define SYS_WRITE0 0x04

int
bea_printk (const char *msg)
{
  return _bea_semihost_rq (0x04, (void *)msg);
}

#undef SYS_WRITE0

void
bea_main ()
{
  for (;;)
    bea_printk ("Hello, world!\n");
}

void
handle_hardfault ()
{
  bea_main ();
}

#define STACK_TOP (void *)(0x20030000)

__attribute__ ((section (".isr_table"), unused)) const static isr_t ISR_TABLE[]
    = { (isr_t)STACK_TOP, bea_main, NULL, handle_hardfault };