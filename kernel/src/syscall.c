#include <driver_table.h>
#include <stdint.h>
#include <syscall.h>

__attribute__ ((interrupt ("SWI"))) void
bea_do_syscall (void)
{
  // Extract syscall num, arguments from R0, R1, R2, and R3
  uint32_t syscall_num;
  uint32_t arg0;
  uint32_t arg1;
  uint32_t arg2;
  uint32_t ret;
  __asm__ volatile ("MRS R0, PSP\n"
                    "LDR R1, [R0]\n"
                    "MOV %[syscall_num], R1\n"
                    "LDR R1, [R0, 4]\n"
                    "MOV %[arg0], R1\n"
                    "LDR R1, [R0, 8]\n"
                    "MOV %[arg1], R1\n"
                    "LDR R1, [R0, 12]\n"
                    "MOV %[arg2], R1\n"
                    : [syscall_num] "=r"(syscall_num), [arg0] "=r"(arg0),
                      [arg1] "=r"(arg1), [arg2] "=r"(arg2)
                    :
                    : "r0", "r1");
  if (syscall_num > BEA_NUM_SYSCALLS)
    {
      return;
    }
  const char *driver_name;
  switch (syscall_num)
    {
    case BEA_SYSCALL_YIELD:
      // Raise a PendSV
      __asm__ volatile ("LDR R0, =0xE000ED04\n"
                        "LDR R2, =(1<<28)\n"
                        "STR R2, [R0]\n"
                        :
                        :
                        : "r0", "r2");
      ret = 0;
      break;
    case BEA_SYSCALL_FIND_DRIVER:
      driver_name = (const char *)arg0;
      ret = (uint32_t)bea_get_driver_by_name (driver_name);
      break;
    case BEA_SYSCALL_DRIVER_RQ:
      if (arg0 == ~0)
        {
          ret = ~0;
        }
      else
        {
          BEA_DRIVER_TABLE[arg0]->request ((void *)arg1, (void *)arg2);
          ret = 1;
        }
    }
  __asm__ volatile ("MRS R0, PSP\n"
                    "STR %[ret], [R0]"
                    :
                    : [ret] "r"(ret)
                    : "r0");
}