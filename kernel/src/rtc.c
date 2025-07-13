#include <pwr.h>
#include <rcc.h>
#include <register_utils.h>
#include <rtc.h>
#include <stddef.h>
#include <stdint.h>

static bool
bea_month_day_is_valid (struct bea_datetime datetime)
{
  // Remember here that `datetime.year` is a `uint8_t` with 0 representing the
  // year 2000
  bool is_leap_year = (datetime.year % 4 == 0)
                      && ((datetime.year % 100 != 0) || (datetime.year == 0));
  enum bea_dotw dotw;
  if (datetime.year == 0)
    {
      // Our upcoming formula breaks for 2000 and 2000 only, let's do something
      // different. The first day of 2000 was a Saturday, and 2000 was a leap
      // year
      uint16_t doty = datetime.day;
      for (size_t i = 0; i < (size_t)datetime.month; ++i)
        {
          doty += BEA_MONTH_LENGTHS_LEAP[i];
        }
      dotw = (enum bea_dotw) (((size_t)BEA_DOTW_SATURDAY + doty - 1) % 7);
    }
  else
    {
      // Use Zeller's congruence. I hate this formula because it's ugly as sin
      // and I had to steal it from Wikipedia, but it is very efficient.
      uint8_t year_adj
          = ((uint8_t)datetime.month < 3) ? datetime.year - 1 : datetime.year;
      uint8_t month_adj = ((uint8_t)datetime.month < 3)
                              ? (uint8_t)datetime.month + 1
                              : (uint8_t)datetime.month + 13;
      uint8_t zeller_dotw = ((datetime.day + (13 * (month_adj + 1)) / 5
                              + (year_adj % 100) + (year_adj % 100) / 4
                              + (year_adj / 100) / 4 + (year_adj / 100) * 5)
                             % 7);
      dotw = (enum bea_dotw) ((zeller_dotw + 5) % 7 + 1);
    }
  uint8_t days_in_month
      = is_leap_year ? BEA_MONTH_LENGTHS_LEAP[(uint8_t)datetime.month]
                     : BEA_MONTH_LENGTHS_NONLEAP[(uint8_t)datetime.month];
  return (datetime.day <= days_in_month && datetime.dotw == dotw);
}

bool
bea_datetime_is_valid (struct bea_datetime datetime)
{
  if (datetime.hour >= 24)
    {
      return false;
    }
  else if (datetime.minute >= 60)
    {
      return false;
    }
  else if (datetime.second >= 60)
    {
      return false;
    }
  return bea_month_day_is_valid (datetime);
}

struct bea_datetime
bea_rtc_get_datetime ()
{
  struct bea_datetime result;
  uint32_t *t_reg = BEA_RTC_BASE_ADDR + BEA_RTC_TR_OFFSET; // Time register
  uint32_t *d_reg = BEA_RTC_BASE_ADDR + BEA_RTC_DR_OFFSET; // Date register
  // Extract the time, which is given in bitpacked BCD format for some
  // God-forsaken reason
  result.second
      = bea_get_reg_bits (t_reg, 3, 0) + 10 * bea_get_reg_bits (t_reg, 7, 4);
  result.minute
      = bea_get_reg_bits (t_reg, 11, 8) + 10 * bea_get_reg_bits (t_reg, 15, 9);
  result.hour = bea_get_reg_bits (t_reg, 19, 16)
                + 10 * bea_get_reg_bits (t_reg, 23, 20);
  if (bea_get_reg_bits (t_reg, 24, 24) != 0)
    {
      result.hour += 12;
    }
  // Extract the date, which is in a similarly God-forsaken format
  result.day
      = bea_get_reg_bits (d_reg, 3, 0) + 10 * bea_get_reg_bits (d_reg, 7, 4);
  result.month
      = (enum bea_month) (bea_get_reg_bits (d_reg, 11, 8)
                          + 10 * bea_get_reg_bits (d_reg, 12, 12) - 1);
  result.dotw = (enum bea_dotw) (bea_get_reg_bits (d_reg, 15, 13) - 1);
  result.year = bea_get_reg_bits (d_reg, 19, 16)
                + 10 * bea_get_reg_bits (d_reg, 23, 20);
  return result;
}

bool
bea_rtc_initialize (enum bea_rtc_clksrc clock_source)
{
  // Set PWR_CR flag so we can write to RTC registers at all
  bea_set_reg_bits (BEA_PWR_BASE_ADDR + BEA_PWR_CR1_OFFSET, 8, 8, 1);
  // We need to initialize a different register in the RCC module than other
  // peripherals. Ain't that a pain?
  uint32_t *bdcr_reg = BEA_RCC_BASE_ADDR + BEA_RCC_BDCR_OFFSET;
  bea_set_reg_bits (bdcr_reg, 15, 15, 1);
  switch (clock_source)
    {
    case BEA_RTC_CLKSRC_HSE:
      bea_set_reg_bits (bdcr_reg, 9, 8, 0b11);
      break;
    case BEA_RTC_CLKSRC_LSE:
      bea_set_reg_bits (bdcr_reg, 9, 8, 0b01);
      break;
    case BEA_RTC_CLKSRC_LSI:
      bea_set_reg_bits (bdcr_reg, 9, 8, 0b10);
      break;
    }
  // Disarm the write protection on the RTC registers
  bea_set_reg_bits (BEA_RTC_BASE_ADDR + BEA_RTC_WPR_OFFSET, 31, 0, 0xCA);
  bea_set_reg_bits (BEA_RTC_BASE_ADDR + BEA_RTC_WPR_OFFSET, 31, 0, 0x53);
  // Set the INIT flag to enter initialization mode
  uint32_t *isr_reg = BEA_RTC_BASE_ADDR + BEA_RTC_ISR_OFFSET;
  bea_set_reg_bits (isr_reg, 7, 7, 1);
  // Poll the INITF flag to wait for initialization mode to begin
  // TODO: add timeout (?)
  while (bea_get_reg_bits (isr_reg, 6, 6) == 0)
    ;
  // Set the prescaler so we can actually measure things
  uint32_t *prescale_reg = BEA_RTC_BASE_ADDR + BEA_RTC_PRER_OFFSET;
  switch (clock_source)
    {
    case BEA_RTC_CLKSRC_HSE:
      // TODO: implement this
      break;
    case BEA_RTC_CLKSRC_LSE:
      bea_set_reg_bits (prescale_reg, 22, 16, 127);
      bea_set_reg_bits (prescale_reg, 14, 0, 255);
      break;
    case BEA_RTC_CLKSRC_LSI:
      // TODO: implement this
      break;
    }
  // Exit initialization mode
  bea_set_reg_bits (isr_reg, 7, 7, 0);
  // Wait for everything to go through(
  // TODO: add timeout (?)
  while (bea_get_reg_bits (isr_reg, 5, 5) == 0)
    ;
  // Re-arm the write protection on the RTC registers
  bea_set_reg_bits (BEA_RTC_BASE_ADDR + BEA_RTC_WPR_OFFSET, 31, 0, 0xFF);
  return true;
}
