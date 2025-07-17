#ifndef BEA_TABLE_H
#define BEA_TABLE_H

#include <driver.h>
#include <stdbool.h>

/// Driver table -- null terminated, each driver *must* have an entry for the
/// kernel to be able to initialize it
const extern struct bea_driver **BEA_DRIVER_TABLE;

#endif
