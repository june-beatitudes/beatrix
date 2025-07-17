#include <bootloader.h>
#include <faults.h>
#include <logging.h>
#include <processor.h>

__attribute__ ((noreturn)) void
bea_do_reset (void)
{
  bea_fpu_enable ();
  bea_log (BEA_LOG_WARNING, "Reset encountered! Was this expected?");
  bea_log (BEA_LOG_INFO, "Branching to bootloader...");
  __asm__ volatile ("MOV R0, %[btl]\n"
                    "BX R0\n"
                    :
                    : [btl] "r"(bea_do_boot)
                    : "r0", "lr", "ip", "memory");
}
