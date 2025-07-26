/**
 * @file
 * @brief Describes the `struct bea_driver` object
 * @author Juniper Beatitudes <juniper@eyes-like-fire.org>
 * @date 2025-07-16
 */

#ifndef BEA_DRIVER_H
#define BEA_DRIVER_H

#include <stdbool.h>
#include <stddef.h>

/// The longest the name of a driver can be (not including the null terminator)
#define BEA_DRIVER_NAME_MAXLEN 31

/**
 * @brief Driver table entry, representing a single driver
 *
 */
struct bea_driver
{
  /// The registered (and unique) name of the driver
  const char name[BEA_DRIVER_NAME_MAXLEN + 1];
  /// Function to initialize the driver. Returns a Boolesn indicating whether or
  /// not initialization was successful
  bool (*initialize) (void);
  /// Function to make a request to the driver from userspace. Not intended for
  /// internal kernel usage
  void (*request) (void *arg, void *result);
  /// Function to deinitialize the driver. Returns a Boolean indicating whether
  /// or not deinitialization was successful
  bool (*deinitialize) (void);
};

#endif
