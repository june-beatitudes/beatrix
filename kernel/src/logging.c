#include <ftoa.h>
#include <logging.h>
#include <processor.h>
#include <rtc.h>
#include <stdbool.h>
#include <string.h>

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
  const char *NEWLINE = "\n";
  char prefixbuf[BEA_DATETIME_KSTRZ_LEN + 12];
  bea_datetime_to_kstrz (bea_rtc_get_datetime (), &prefixbuf[0]);
  prefixbuf[BEA_DATETIME_KSTRZ_LEN] = ' ';
  bea_strncpy (BEA_LOG_LEVEL_NAMES[(size_t)level],
               &prefixbuf[BEA_DATETIME_KSTRZ_LEN + 1], 7);
  prefixbuf[BEA_DATETIME_KSTRZ_LEN + 8] = ' ';
  prefixbuf[BEA_DATETIME_KSTRZ_LEN + 9] = '\0';
  bea_semihost_rq (BEA_SEMIHOST_SYSWRITEZ, (void *)prefixbuf);
  bea_semihost_rq (BEA_SEMIHOST_SYSWRITEZ, (void *)msg);
  bea_semihost_rq (BEA_SEMIHOST_SYSWRITEZ, (void *)NEWLINE);
  return true;
#endif
  return false;
}
