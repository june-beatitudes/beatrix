#include <logging.h>

void
shell_main (void)
{
  for (;;)
    {
      bea_log (BEA_LOG_INFO, "Hello, world!");
    }
}