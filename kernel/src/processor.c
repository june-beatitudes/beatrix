#include <processor.h>
#include <stdint.h>

void
bea_fpu_enable ()
{
  // Code taken directly from STMicroelectronics PM0214, Rev. 10, pp. 257
  __asm__ volatile ("LDR.W R0, =0xE000ED88 \n"
                    "LDR R1, [R0] \n"
                    "ORR R1, R1, #(0xF << 20) \n"
                    "STR R1, [R0] \n"
                    "DSB \n"
                    "ISB"
                    :
                    :
                    : "memory", "r0", "r1");
}

__attribute__ ((always_inline)) inline int32_t
bea_semihost_rq (int32_t reason, void *arg)
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
