#include <ftoa.h>
#include <logging.h>
#include <processor.h>
#include <rtc.h>
#include <stdbool.h>

bool
bea_log (enum bea_log_level_t level, const char *msg)
{
#ifndef DEBUG
  if (level == BEA_LOG_DEBUG)
    {
      return false;
    }
#endif
#ifdef SEMIHOSTING
  const char *SPACE = " ";
  const char *NEWLINE = "\n";
  char datebuf[BEA_DATETIME_KSTRZ_LEN];
  bea_datetime_to_kstrz (bea_rtc_get_datetime (), &datebuf[0]);
  bea_semihost_rq (BEA_SEMIHOST_SYSWRITEZ, (void *)datebuf);
  bea_semihost_rq (BEA_SEMIHOST_SYSWRITEZ, (void *)SPACE);
  bea_semihost_rq (BEA_SEMIHOST_SYSWRITEZ,
                   (void *)BEA_LOG_LEVEL_NAMES[(size_t)level]);
  bea_semihost_rq (BEA_SEMIHOST_SYSWRITEZ, (void *)SPACE);
  bea_semihost_rq (BEA_SEMIHOST_SYSWRITEZ, (void *)msg);
  bea_semihost_rq (BEA_SEMIHOST_SYSWRITEZ, (void *)NEWLINE);
  return true;
#endif
  return false;
}
