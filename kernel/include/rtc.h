#ifndef BEATRIX_RTC_H
#define BEATRIX_RTC_H

#include <common.h>

#define BEA_RTC_BASE_ADDR (uint32_t *)(0x40002800)

#define BEA_RTC_ISR_OFFSET 0x03
#define BEA_RCC_BDCR_OFFSET 0x024
#define BEA_RTC_PRER_OFFSET 0x04
#define BEA_RTC_WPR_OFFSET 0x09
/**
 * @brief Represents a clock source for the RTC subsystem to use (see RM0434
 * pp. 263)
 *
 */
enum bea_rtc_clksrc
{
  BEA_RTC_CLKSRC_LSE,
  BEA_RTC_CLKSRC_LSI,
  BEA_RTC_CLKSRC_HSE,
};

/**
 * @brief Initialize the RTC subsystem.
 *
 * @param clock_source Clock source for the RTC subsystem to use (see RM0434
 * pp. 263)
 * @return true No error encountered
 * @return false Error encountered
 */
bool bea_rtc_initialize (enum bea_rtc_clksrc clock_source);

#define BEA_RTC_TR_OFFSET 0x00
uint32_t bea_rtc_get_seconds ();

#endif