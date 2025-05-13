#ifndef BEA_DRIVERS_TABLE_H
#define BEA_DRIVERS_TABLE_H

#define DRIVER_NAME_LEN_MAX 16

#include "../include/common.h"

struct bea_driver
{
        const char name[DRIVER_NAME_LEN_MAX];
        bea_err_t (*init) (void);
        bea_err_t (*deinit) (void);
        size_t (*read) (const char *fname, char *buf, size_t n);
        size_t (*write) (const char *fname, char *buf, size_t n);
};

#endif