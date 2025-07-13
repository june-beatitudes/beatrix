#include <ftoa.h>
#include <isr.h>
#include <logging.h>
#include <processor.h>
#include <rcc.h>
#include <rtc.h>
#include <stdbool.h>
#include <stdlib.h>

void
bea_string_copy (const char *from, char *to, size_t n)
{
  for (size_t i = 0; i < n; ++i)
    {
      to[i] = from[i];
    }
}

void
bea_main ()
{
  bea_fpu_enable ();
  struct bea_datetime foo = {
    .year = 25,
    .month = BEA_MONTH_JULY,
    .day = 12,
    .dotw = BEA_DOTW_SATURDAY,
    .hour = 21,
    .minute = 30,
    .second = 0.0f,
  };
  bea_rtc_initialize (BEA_RTC_CLKSRC_LSE, foo, 256);
  for (;;)
    {
      bea_log (BEA_LOG_INFO, "Hello, world!");
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
