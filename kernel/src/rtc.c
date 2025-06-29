#include <common.h>
#include <rcc.h>
#include <rtc.h>

bool
bea_rtc_initialize (enum bea_rtc_clksrc clock_source)
{
  // Set PWR_CR flag so we can write to RTC registers at all
  *((uint32_t *)0x58000400) |= (1 << 8);
  // We need to initialize a different register in the RCC module than other
  // peripherals. Ain't that a pain?
  *(BEA_RCC_BASE_ADDR + BEA_RCC_BDCR_OFFSET) |= (1 << 15);
  switch (clock_source)
    {
    case BEA_RTC_CLKSRC_HSE:
      *(BEA_RCC_BASE_ADDR + BEA_RCC_BDCR_OFFSET) |= (0b11 << 8);
      break;
    case BEA_RTC_CLKSRC_LSE:
      *(BEA_RCC_BASE_ADDR + BEA_RCC_BDCR_OFFSET) &= ~(0b10 << 8);
      *(BEA_RCC_BASE_ADDR + BEA_RCC_BDCR_OFFSET) |= (0b01 << 8);
      break;
    case BEA_RTC_CLKSRC_LSI:
      *(BEA_RCC_BASE_ADDR + BEA_RCC_BDCR_OFFSET) &= ~(0b01 << 8);
      *(BEA_RCC_BASE_ADDR + BEA_RCC_BDCR_OFFSET) |= (0b10 << 8);
      break;
    }
  // Disarm the write protection on the RTC registers
  *(BEA_RTC_BASE_ADDR + 0x09) = 0xCA;
  *(BEA_RTC_BASE_ADDR + 0x09) = 0x53;
  // Set the INIT flag to enter initialization mode
  *(BEA_RTC_BASE_ADDR + BEA_RTC_ISR_OFFSET) |= (1 << 7);
  // Poll the INITF flag to wait for initialization mode to begin
  while (!(*(BEA_RTC_BASE_ADDR + BEA_RTC_ISR_OFFSET) & (1 << 6)))
    ;
  // Set the prescaler so we can actually measure things
  *(BEA_RTC_BASE_ADDR + BEA_RTC_PRER_OFFSET) &= ~0x7FFF;
  switch (clock_source)
    {
    case BEA_RTC_CLKSRC_HSE:
      *(BEA_RTC_BASE_ADDR + BEA_RTC_PRER_OFFSET) |= (1499999);
      break;
    case BEA_RTC_CLKSRC_LSE:
      *(BEA_RTC_BASE_ADDR + BEA_RTC_PRER_OFFSET) &= ~(0x3FFF);
      *(BEA_RTC_BASE_ADDR + BEA_RTC_PRER_OFFSET) |= (255);
      break;
    case BEA_RTC_CLKSRC_LSI:
      *(BEA_RTC_BASE_ADDR + BEA_RTC_PRER_OFFSET) |= (31999);
      break;
    }
  // Exit initialization mode
  *(BEA_RTC_BASE_ADDR + BEA_RTC_ISR_OFFSET) &= ~(1 << 7);
  // Wait for everything to go through
  while (!(*(BEA_RTC_BASE_ADDR + BEA_RTC_ISR_OFFSET) & (1 << 5)))
    ;
  // Re-arm the write protection on the RTC registers
  *(BEA_RTC_BASE_ADDR + 0x09) = 0xFF;
  return true;
}

uint32_t
bea_rtc_get_seconds ()
{
  return *(BEA_RTC_BASE_ADDR + BEA_RTC_TR_OFFSET) & 0xF;
}