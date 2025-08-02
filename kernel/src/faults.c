#include <bootloader.h>
#include <faults.h>
#include <logging.h>
#include <processor.h>

__attribute__ ((noreturn)) void
bea_do_reset (void)
{
  // If we don't enable the FPU then logging will fail immediately
  bea_fpu_enable ();
  bea_log (BEA_LOG_WARNING, "Reset encountered! Was this expected?");
  bea_log (BEA_LOG_INFO, "Branching to bootloader...");
  bea_do_boot();
}
