#include <processor.h>

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
