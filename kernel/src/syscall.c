#include <stdint.h>
#include <syscall.h>

__attribute__ ((interrupt ("SWI"))) void
bea_do_syscall (void)
{
  // Extract syscall num from the preserved PC
  uint32_t syscall_num;
  __asm__ volatile ("MRS R0, PSP\n"
                    "LDR R1, [R0, 24]\n"
                    "SUB R1, 2\n"
                    "LDRH R1, [R1]\n"
                    "UBFX R1, R1, 0, 8\n"
                    "MOV %[syscall_num], R1\n"
                    : [syscall_num] "=r"(syscall_num)
                    :
                    : "r0", "r1");
  if (syscall_num > BEA_NUM_SYSCALLS)
    {
      return;
    }
  switch (syscall_num)
    {
    case BEA_SYSCALL_YIELD:
      __asm__ volatile ("LDR R0, =0xE000ED04\n"
                        "LDR R2, =(1<<28)\n"
                        "STR R2, [R0]\n"
                        :
                        :
                        : "r0", "r2");
      break;
    }
}