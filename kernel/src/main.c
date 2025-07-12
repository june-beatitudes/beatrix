#include <isr.h>
#include <rcc.h>
#include <rtc.h>
#include <stdlib.h>

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
bea_string_copy (const char *from, char *to, size_t n)
{
  for (size_t i = 0; i < n; ++i)
    {
      to[i] = from[i];
    }
}

const static char BEA_WEEKDAY_NAMES[7][4] = {
  "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN",
};

const static char BEA_MONTH_NAMES[12][4] = {
  "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
  "JUL", "AUG", "SEP", "OCT", "NOV", "DEC",
};

bool
bea_krnl_datetime_to_str (const struct bea_datetime datetime, char *buf)
{
  if (!bea_datetime_is_valid (datetime))
    {
      // return false;
    }
  char *cursor = buf;
  bea_string_copy (BEA_WEEKDAY_NAMES[(size_t)datetime.dotw], cursor, 3);
  cursor += 3;
  *cursor = ' ';
  cursor++;
  bea_string_copy (BEA_MONTH_NAMES[(size_t)datetime.month], cursor, 3);
  cursor += 3;
  *cursor = ' ';
  cursor++;
  *cursor = '0' + datetime.day / 10;
  cursor++;
  *cursor = '0' + datetime.day % 10;
  cursor++;
  bea_string_copy (" 20", cursor, 3);
  cursor += 3;
  *cursor = '0' + datetime.year / 10;
  cursor++;
  *cursor = '0' + datetime.year % 10;
  cursor++;
  *cursor = ' ';
  cursor++;
  *cursor = '0' + datetime.hour / 10;
  cursor++;
  *cursor = '0' + datetime.hour % 10;
  cursor++;
  *cursor = ':';
  cursor++;
  *cursor = '0' + datetime.minute / 10;
  cursor++;
  *cursor = '0' + datetime.minute % 10;
  cursor++;
  *cursor = ':';
  cursor++;
  *cursor = '0' + datetime.second / 10;
  cursor++;
  *cursor = '0' + datetime.second % 10;
  cursor++;
  bea_string_copy (" UTC", cursor, 5);
  return true;
}

void
bea_main ()
{
  bea_rtc_initialize (BEA_RTC_CLKSRC_LSE);
  char buf[29];
  for (;;)
    {
      struct bea_datetime now = bea_rtc_get_datetime ();
      bea_krnl_datetime_to_str (now, buf);
      bea_printk (buf);
      bea_printk ("\n");
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
