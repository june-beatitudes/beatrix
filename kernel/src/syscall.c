#include <logging.h>
#include <stdint.h>
#include <syscall.h>

void
bea_do_syscall (void)
{
  // Retrieve the syscall number from R0, argument 1 from R1, argument 2 from
  // R2, argument 3 from R3
  uint32_t reason;
  uint32_t argument1;
  uint32_t argument2;
  uint32_t argument3;
  __asm__ volatile ("MOV %[rsn], R0\n"
                    "MOV %[arg1], R1\n"
                    "MOV %[arg2], R2\n"
                    "MOV %[arg3], R3\n"
                    : [rsn] "=r"(reason), [arg1] "=r"(argument1),
                      [arg2] "=r"(argument2), [arg3] "=r"(argument3)
                    :
                    :);
  bea_log (BEA_LOG_DEBUG, "Received a system call");
  uint32_t retval;
  if (reason >= (uint32_t)BEA_NUM_SYSCALLS)
    {
      bea_log (BEA_LOG_ERROR, "System call was invalid (nonexistent reason)");
      retval = 0xFFFFFFFF;
    }
  else
    {
      retval = 0x0;
    }
  __asm__ volatile ("MOV R0, %[ret]\n" : : [ret] "r"(retval) :);
}
