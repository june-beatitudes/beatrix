#include <logging.h>
#include <processor.h>
#include <register_utils.h>
#include <stdint.h>
#include <syscall.h>
#include <driver_table.h>

__attribute__ ((interrupt ("SWI"))) void
bea_do_syscall (void)
{
  uint32_t *stack_pointer;
  __asm__ volatile ("MOV [sp], SP" : [sp] "=r"(stack_pointer));
  // Turn off the SysTick timer
  bea_set_reg_bits ((uint32_t *)BEA_STK_CTRL, BEA_STK_ENABLE, BEA_STK_ENABLE,
                    0);
  // Retrieve the syscall number from R0, argument 1 from R1, argument 2 from
  // R2, argument 3 from R3
  uint32_t reason = stack_pointer[0];
  uint32_t arg1 = stack_pointer[1];
  uint32_t arg2 = stack_pointer[2];
  uint32_t arg3 = stack_pointer[3];
  bea_log (BEA_LOG_DEBUG, "Received a system call");
  uint32_t retval;
  if (reason >= (uint32_t)BEA_NUM_SYSCALLS)
    {
      bea_log (BEA_LOG_ERROR, "System call was invalid (nonexistent reason)");
      retval = ~0;
    }
  else
    {
      struct bea_driver *driver; // Used for driver calls
      switch ((enum bea_syscall)reason)
        {
        case BEA_SYSCALL_LOG:
          bea_log ((enum bea_log_level_t)arg1, (const char *)arg2);
          retval = 0x0;
          break;
        case BEA_SYSCALL_FIND_DRIVER:
          retval = (uint32_t) bea_get_driver_by_name((const char *)arg1);
          break;
        case BEA_SYSCALL_DRIVER_RQ:
          driver = (struct bea_driver *)arg1;
          driver->request((void *)arg2, (void *)arg3);
          retval = 0x0;
          break;
        case BEA_SYSCALL_EXECUTE:
        case BEA_SYSCALL_SPAWN:
        case BEA_SYSCALL_KILL:
        case BEA_SYSCALL_SILENCE_RQ:
        case BEA_SYSCALL_YIELD:
          // TODO: implement !!
          retval = ~0;
          break;
        }
    }
  stack_pointer[0] = retval;
  // Turn SysTick back on
  bea_set_reg_bits ((uint32_t *)BEA_STK_CTRL, BEA_STK_ENABLE, BEA_STK_ENABLE,
                    1);
}
