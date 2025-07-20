#include <ftoa.h>
#include <math.h>
#include <pwr.h>
#include <rcc.h>
#include <register_utils.h>
#include <rtc.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static bool
bea_month_day_is_valid (struct bea_datetime datetime)
{
  // Remember here that `datetime.year` is a `uint8_t` with 0 representing the
  // year 2000
  bool is_leap_year
      = (datetime.year % 4 == 0) && ((datetime.year % 100 != 0) || (datetime.year == 0));
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
      uint8_t year_adj = ((uint8_t)datetime.month < 3) ? datetime.year - 1 : datetime.year;
      uint8_t month_adj = ((uint8_t)datetime.month < 3) ? (uint8_t)datetime.month + 1
                                                        : (uint8_t)datetime.month + 13;
      uint8_t zeller_dotw = ((datetime.day + (13 * (month_adj + 1)) / 5 + (year_adj % 100)
                              + (year_adj % 100) / 4 + (year_adj / 100) / 4 + (year_adj / 100) * 5)
                             % 7);
      dotw = (enum bea_dotw) ((zeller_dotw + 5) % 7 + 1);
    }
  uint8_t days_in_month = is_leap_year ? BEA_MONTH_LENGTHS_LEAP[(uint8_t)datetime.month]
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
  else if (datetime.second >= 60.0f)
    {
      return false;
    }
  return bea_month_day_is_valid (datetime);
}

/// Names of weekdays in ISO order, truncated to 3 characters
const static char BEA_WEEKDAY_NAMES[7][4] = {
  "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN",
};

/// Names of months in ISO order, truncated to 3 characters
const static char BEA_MONTH_NAMES[12][4] = {
  "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC",
};

void
bea_datetime_to_kstrz (const struct bea_datetime datetime, char *buf)
{
  char *cursor = buf;
  bea_strncpy (BEA_WEEKDAY_NAMES[(size_t)datetime.dotw], cursor, 3);
  cursor += 3;
  *cursor = ' ';
  cursor++;
  bea_strncpy (BEA_MONTH_NAMES[(size_t)datetime.month], cursor, 3);
  cursor += 3;
  *cursor = ' ';
  cursor++;
  *cursor = '0' + datetime.day / 10;
  cursor++;
  *cursor = '0' + datetime.day % 10;
  cursor++;
  bea_strncpy (" 20", cursor, 3);
  cursor += 3;
  *cursor = '0' + datetime.year / 10;
  cursor++;
  *cursor = '0' + datetime.year % 10;
  cursor++;
  *cursor = ' ';
  cursor++;
  *cursor = '0' + datetime.hour / 10;
  cursor++;
  *cursor = '0' + datetime.hour % 10;
  cursor++;
  *cursor = ':';
  cursor++;
  *cursor = '0' + datetime.minute / 10;
  cursor++;
  *cursor = '0' + datetime.minute % 10;
  cursor++;
  *cursor = ':';
  cursor++;
  bea_ftoa (datetime.second, cursor, 6, false);
  cursor += 6;
  bea_strncpy (" UTC", cursor, 5);
}

struct bea_datetime
bea_rtc_get_datetime ()
{
  struct bea_datetime result;
  uint32_t *t_reg = BEA_RTC_BASE_ADDR + BEA_RTC_TR_OFFSET; // Time register
  uint32_t *d_reg = BEA_RTC_BASE_ADDR + BEA_RTC_DR_OFFSET; // Date register

  uint32_t second_integral = (bea_get_reg_bits (t_reg, 3, 0) + 10 * bea_get_reg_bits (t_reg, 6, 4));
  uint32_t prescaler_sync_factor
      = bea_get_reg_bits (BEA_RTC_BASE_ADDR + BEA_RTC_PRER_OFFSET, 14, 0);
  float second_fractional
      = ((float)prescaler_sync_factor
         - (float)bea_get_reg_bits (BEA_RTC_BASE_ADDR + BEA_RTC_SSR_OFFSET, 15, 0))
        / ((float)prescaler_sync_factor + 1.0f);
  result.second = (float)second_integral + second_fractional;

  result.minute = bea_get_reg_bits (t_reg, 11, 8) + 10 * bea_get_reg_bits (t_reg, 14, 12);
  result.hour = bea_get_reg_bits (t_reg, 19, 16) + 10 * bea_get_reg_bits (t_reg, 21, 20);
  if (bea_get_reg_bits (t_reg, 22, 22) != 0)
    {
      result.hour += 12;
    }
  // Extract the date, which is in a similarly God-forsaken format
  result.day = bea_get_reg_bits (d_reg, 3, 0) + 10 * bea_get_reg_bits (d_reg, 5, 4);
  result.month = (enum bea_month) (bea_get_reg_bits (d_reg, 11, 8)
                                   + 10 * bea_get_reg_bits (d_reg, 12, 12) - 1);
  result.dotw = (enum bea_dotw) (bea_get_reg_bits (d_reg, 15, 13) - 1);
  result.year = bea_get_reg_bits (d_reg, 19, 16) + 10 * bea_get_reg_bits (d_reg, 23, 20);
  return result;
}

static bool
bea_rtc_init_prescaler (enum bea_rtc_clksrc clock_source, uint32_t subsec_freq)
{
  float rtc_clock_freq = 32768.0f;
  switch (clock_source)
    {
    case BEA_RTC_CLKSRC_HSE:
      rtc_clock_freq = 1000000.0f;
      break;
    case BEA_RTC_CLKSRC_LSE:
      rtc_clock_freq = 32768.0f;
      break;
    case BEA_RTC_CLKSRC_LSI:
      rtc_clock_freq = 32000.0f;
      break;
    }
  // Either of these will indicate that the subsecond frequency is just too
  // fast
  if ((float)subsec_freq > rtc_clock_freq)
    {
      return false;
    }
  else if (subsec_freq - 1 >= (1 << 15))
    {
      return false;
    }
  bea_set_reg_bits (BEA_RTC_BASE_ADDR + BEA_RTC_PRER_OFFSET, 14, 0, subsec_freq - 1);
  float async_factor_f = (rtc_clock_freq / subsec_freq) - 1.0f;
  if (async_factor_f >= (float)(1 << 7))
    {
      return false;
    }
  uint32_t async_factor_u = (uint32_t)lroundf (async_factor_f);
  bea_set_reg_bits (BEA_RTC_BASE_ADDR + BEA_RTC_PRER_OFFSET, 22, 16, async_factor_u);
  return true;
}

static bool
bea_rtc_set_datetime (struct bea_datetime datetime)
{
  if (!bea_datetime_is_valid (datetime))
    {
      // return false;
    }
  uint32_t *t_reg = BEA_RTC_BASE_ADDR + BEA_RTC_TR_OFFSET; // Time register
  uint32_t *d_reg = BEA_RTC_BASE_ADDR + BEA_RTC_DR_OFFSET; // Date register

  uint32_t nearest_second = lroundf (floorf (datetime.second));
  bea_set_reg_bits (t_reg, 3, 0, nearest_second % 10);
  bea_set_reg_bits (t_reg, 6, 4, nearest_second / 10);
  bea_set_reg_bits (t_reg, 11, 8, datetime.minute % 10);
  bea_set_reg_bits (t_reg, 14, 12, datetime.minute / 10);
  bea_set_reg_bits (t_reg, 19, 16, datetime.hour % 10);
  bea_set_reg_bits (t_reg, 21, 20, datetime.hour / 10);
  bea_set_reg_bits (t_reg, 22, 22, 0);

  uint32_t d_reg_val = 0x0;
  bea_set_reg_bits (&d_reg_val, 3, 0, datetime.day % 10);
  bea_set_reg_bits (&d_reg_val, 5, 4, datetime.day / 10);
  uint32_t month_adjusted = (uint32_t)datetime.month + 1;
  bea_set_reg_bits (&d_reg_val, 11, 8, month_adjusted % 10);
  bea_set_reg_bits (&d_reg_val, 12, 12, month_adjusted / 10);
  uint32_t dotw_adjusted = (uint32_t)datetime.dotw + 1;
  bea_set_reg_bits (&d_reg_val, 15, 13, dotw_adjusted);
  bea_set_reg_bits (&d_reg_val, 19, 16, datetime.year % 10);
  bea_set_reg_bits (&d_reg_val, 23, 20, datetime.year / 10);
  bea_set_reg_bits (d_reg, 31, 0, d_reg_val);

  return true;
}

bool
bea_rtc_initialize (enum bea_rtc_clksrc clock_source, struct bea_datetime start_time,
                    uint32_t subsec_freq)
{
  uint32_t *bdcr_reg = BEA_RCC_BASE_ADDR + BEA_RCC_BDCR_OFFSET;
  // Set PWR_CR flag so we can write to RTC registers at all
  uint32_t *pwr_reg = BEA_PWR_BASE_ADDR + BEA_PWR_CR1_OFFSET;
  bea_set_reg_bits (pwr_reg, 8, 8, 1);
  bea_set_reg_bits (bdcr_reg, 16, 16, 1);
  bea_set_reg_bits (pwr_reg, 8, 8, 1);
  bea_set_reg_bits (bdcr_reg, 16, 16, 0);
  // We need to initialize a different register in the RCC module than other
  // peripherals. Ain't that a pain?
  bea_set_reg_bits (bdcr_reg, 15, 15, 1);
  switch (clock_source)
    {
    case BEA_RTC_CLKSRC_HSE:
      bea_set_reg_bits (bdcr_reg, 9, 8, 0b11);
      break;
    case BEA_RTC_CLKSRC_LSE:
      // Make sure the LSE is turned on
      bea_set_reg_bits (bdcr_reg, 9, 8, 0b01);
      bea_set_reg_bits (bdcr_reg, 0, 0, 1);
      while (bea_get_reg_bits (bdcr_reg, 1, 1) == 0)
        ;
      break;
    case BEA_RTC_CLKSRC_LSI:
      bea_set_reg_bits (bdcr_reg, 9, 8, 0b10);
      break;
    }
  // Disarm the write protection on the RTC registers
  uint32_t *wpr_reg = BEA_RTC_BASE_ADDR + BEA_RTC_WPR_OFFSET;
  bea_set_reg_bits (wpr_reg, 31, 0, 0xCA);
  bea_set_reg_bits (wpr_reg, 31, 0, 0x53);
  // Set the INIT flag to enter initialization mode
  uint32_t *isr_reg = BEA_RTC_BASE_ADDR + BEA_RTC_ISR_OFFSET;
  bea_set_reg_bits (isr_reg, 7, 7, 1);
  bea_set_reg_bits (isr_reg, 5, 5, 0);
  // Poll the INITF flag to wait for initialization mode to begin
  while (bea_get_reg_bits (isr_reg, 6, 6) == 0)
    ;
  // Set the prescaler so we can actually measure things
  bea_rtc_init_prescaler (clock_source, subsec_freq);
  // Set the start time
  bea_rtc_set_datetime (start_time);
  // Exit initialization mode
  bea_set_reg_bits (isr_reg, 7, 7, 0);
  // Wait for everything to go through
  while (bea_get_reg_bits (isr_reg, 5, 5) == 0)
    ;
  // Re-arm the write protection on the RTC registers
  bea_set_reg_bits (wpr_reg, 31, 0, 0xFF);
  return true;
}
