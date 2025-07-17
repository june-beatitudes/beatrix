#include <driver_table.h>
#include <gpio/gpio.h>

const struct bea_driver BEA_DRIVER_END_SENTINEL = { .name = "" };

const struct bea_driver *const BEA_DRIVER_TABLE[] = { &BEA_GPIO_DRIVER, NULL };
