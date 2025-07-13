#ifndef BEA_KERNEL_LOGGING_H
#define BEA_KERNEL_LOGGING_H

#include <ftoa.h>
#include <processor.h>
#include <rtc.h>
#include <stdbool.h>

/**
 * @brief Kernel logging levels
 */
enum bea_log_level_t
{
  /// Only for debugging
  BEA_LOG_DEBUG = 0,
  /// General information
  BEA_LOG_INFO,
  /// Warnings that do not constitute errors
  BEA_LOG_WARNING,
  /// Errors that are recoverable / ignorable
  BEA_LOG_ERROR,
  /// Errors that cannot be recovered from
  BEA_LOG_FATAL,
};

static const char BEA_LOG_LEVEL_NAMES[5][8] = {
  "DEBUG  ", "INFO   ", "WARNING", "ERROR  ", "FATAL  ",
};

/**
 * @brief Log message to whatever log output device has been specified at build
 * time
 *
 * @param level The log level
 * @param msg The log message
 *
 * @return true Logging was actually performed (device is connected and log
 * level is set appropriately)
 * @return false Logging was not performed
 */
bool bea_log (enum bea_log_level_t level, const char *msg);

#endif
