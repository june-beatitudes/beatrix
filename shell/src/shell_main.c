#include <logging.h>

void
shell_main (__attribute__ ((unused)) void *_)
{
  for (;;)
    {
      bea_log (BEA_LOG_INFO, "Hello, world 1!");
    }
}