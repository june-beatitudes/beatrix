#include <logging.h>
#include <stdint.h>
#include <syscall.h>

void
bea_do_syscall (void)
{
  // Retrieve the syscall number from R0, argument from R1
  uint32_t reason;
  uint32_t argument;
  __asm__ volatile ("MOV %[rsn], R0\n"
                    "MOV %[arg], R1\n"
                    : [rsn] "=r"(reason), [arg] "=r"(argument)
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
      // TODO: dispatch syscall
    }
  __asm__ volatile ("MOV R0, %[ret]\n" : : [ret] "r"(retval) :);
}
