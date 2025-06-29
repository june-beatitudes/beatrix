#include <common.h>
#include <isr.h>
#include <rtc.h>

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
  bea_rtc_initialize (BEA_RTC_CLKSRC_LSE);
  const char msg_flash[] = "0 Hello, world!\n";
  char *msg = (char *)0x20000000;
  for (size_t i = 0; i < sizeof (msg_flash); ++i)
    {
      msg[i] = msg_flash[i];
    }
  for (;;)
    {
      msg[0] = '0' + bea_rtc_get_seconds ();
      bea_printk (msg);
    }
}

void
handle_hardfault ()
{
  bea_main ();
}

#define STACK_TOP (void *)(0x20030000)

__attribute__ ((section (".isr_table"), unused)) const static isr_t ISR_TABLE[]
    = { (isr_t)STACK_TOP, bea_main, NULL, handle_hardfault };