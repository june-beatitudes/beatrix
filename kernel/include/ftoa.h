/**
 * @file
 * @brief Functionality for converting floats to strings inside the kernel,
 * used primarily for logging
 * @author Juniper Beatitudes
 */

#ifndef BEA_FTOA_H
#define BEA_FTOA_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief The result indicator of the `bea_ftoa` function
 */
enum bea_ftoa_result
{
  /// A successful run with no warnings
  BEA_FTOA_SUCCESS,
  /// An unsuccessful run, not enough space in the buffer to
  /// represent the number as specified
  BEA_FTOA_ENOSPACE,
  /// A successful run, but more significant figures
  /// were emitted than the number meaningfully has
  BEA_FTOA_WPRECISIONLOSS,
};

/**
 * @brief Fills a buffer with a string representation of a single-precision
 * IEEE-754 floating point number
 *
 * @param x The number to stringify
 * @param buf The output buffer
 * @param n_available The number of characters available in the output buffer;
 * all characters will be used
 * @param include_positive_sign Boolean indicating whether or not a sign
 * character should be emitted if `x` is positive
 *
 * @return BEA_FTOA_SUCCESS Represents a fully successful run with no
 * loss-of-precision warning
 * @return BEA_FTOA_ENOSPACE Indicates that there is insufficient space in the
 * buffer to represent the integral portion of the number
 * @return BEA_FTOA_WPRECISIONLOSS Indicates that there are more significant
 * digits emitted than a single-precision floating point number actually has
 *
 * @todo Add support for emitting INF, NaN
 * @todo Add support for scientific notation
 */
enum bea_ftoa_result bea_ftoa (float x, char *buf, size_t n_available,
                               bool include_positive_sign);

#endif
