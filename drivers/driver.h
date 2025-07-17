/**
 * @file
 * @brief Describes the `struct bea_driver` object
 * @author Juniper Beatitudes
 * @date 2025-07-16
 */

#ifndef BEA_DRIVER_H
#define BEA_DRIVER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// The longest the name of a driver can be (not including the null terminator)
#define BEA_DRIVER_NAME_MAXLEN 31

/**
 * @brief Driver table entry, representing a single driver
 *
 * @member name The registered *unique* name of the driver
 * @member initialize Initializes necessary functionality for the driver and
 * returns a boolean indicating if initialization was successful or not
 * @member request The request method, which takes in an argument, performs
 * some action, and fills out a result object. Returns a boolean indicating if
 * the operation was successful or not
 * @member deinitialize Deinitializes functionality for the driver and returns
 * a bvoolean indicating if that operation was successful or not
 */
struct bea_driver
{
  const char name[BEA_DRIVER_NAME_MAXLEN + 1];
  bool (*initialize) (void);
  bool (*request) (void *arg, void *result);
  bool (*deinitialize) (void);
};

#endif
