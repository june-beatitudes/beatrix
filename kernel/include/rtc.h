#ifndef BEATRIX_RTC_H
#define BEATRIX_RTC_H

#include <stdbool.h>
#include <stdint.h>

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

/**
 * @brief Represents a day of the week in the Gregorian calendar
 *
 */
enum bea_dotw
{
  BEA_DOTW_MONDAY = 0,
  BEA_DOTW_TUESDAY,
  BEA_DOTW_WEDNESDAY,
  BEA_DOTW_THURSDAY,
  BEA_DOTW_FRIDAY,
  BEA_DOTW_SATURDAY,
  BEA_DOTW_SUNDAY,
};

/**
 * @brief Represents a month in the Gregorian calendar
 *
 */
enum bea_month
{
  BEA_MONTH_JANUARY = 0,
  BEA_MONTH_FEBRUARY,
  BEA_MONTH_MARCH,
  BEA_MONTH_APRIL,
  BEA_MONTH_MAY,
  BEA_MONTH_JUNE,
  BEA_MONTH_JULY,
  BEA_MONTH_AUGUST,
  BEA_MONTH_SEPTEMBER,
  BEA_MONTH_OCTOBER,
  BEA_MONTH_NOVEMBER,
  BEA_MONTH_DECEMBER,
};

const static uint8_t BEA_MONTH_LENGTHS_NONLEAP[12]
    = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const static uint8_t BEA_MONTH_LENGTHS_LEAP[12]
    = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/**
 * @brief Represents a single point in time in the Gregorian calendar. Years
 * begin at 2000 (since the RTC hardware can't represent anything earlier than
 * that), and hours are in 24-hour time. Always in UTC because the IANA
 * timezone database is in itself bigger than the amount of flash we have to
 * work with
 *
 */
struct bea_datetime
{
  uint8_t year;
  enum bea_month month;
  uint8_t day;
  enum bea_dotw dotw;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

/**
 * @brief Checks whether a `struct bea_datetime` represents a valid point in
 * time in the Gregorian calendar
 *
 * @param datetime The datetime to check
 * @return true The represented datetime is valid
 * @return false The represented datetime is invalid
 */
bool bea_datetime_is_valid (struct bea_datetime datetime);

/**
 * @brief Converts a standard Unix timestamp (seconds since the beginning of
 * the year 1970) to a `bea_datetime`
 *
 * @param timestamp The timestamp in Unix format
 * @param datetime_out A pointer to an uninitialized `bea_datetime` object,
 * which will be filled out by the function
 * @return true The conversion was successful
 * @return false The conversion was unsuccessful due to a mismatch in
 * representable ranges
 */
bool bea_unix_to_datetime (uint64_t timestamp,
                           struct bea_datetime *datetime_out);

/**
 * @brief converts a `bea_datetime` to a Unix timestamp (seconds since the
 * beginning of the year 1970)
 *
 * @param datetime The datetime in `bea_datetime` format
 * @param timestamp_out A pointer to an uninitialized `uint64_t`, whose value
 * will be filled in by the function
 * @return true The conversion was successful
 * @return false The conversion was unsuccessful because `datetime` is invalid
 */
bool bea_datetime_to_unix (struct bea_datetime datetime,
                           uint64_t timestamp_out);

#define BEA_RTC_TR_OFFSET 0x00
#define BEA_RTC_DR_OFFSET 0x01
/**
 * @brief Reads the current date and time according to the RTC
 *
 * @return struct bea_datetime The returned date and time values
 */
struct bea_datetime bea_rtc_get_datetime ();

#endif
