/**
 * @file
 * @brief Contains the declaration for the driver table
 * @author Juniper Beatitudes <juniper@eyes-like-fire.org>
 * @date 2025-07-16
 */

#ifndef BEA_TABLE_H
#define BEA_TABLE_H

#include <driver.h>
#include <stdbool.h>

/// Driver table -- null terminated, each driver *must* have an entry for the
/// kernel to be able to initialize it
const extern struct bea_driver *const BEA_DRIVER_TABLE[];

const struct bea_driver *const bea_get_driver_by_name (const char *name);

#endif
