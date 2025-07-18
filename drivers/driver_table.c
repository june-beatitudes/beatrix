#include <driver_table.h>
#include <gpio/gpio.h>
#include <sd/sd.h>
#include <spi/spi.h>

const struct bea_driver *const BEA_DRIVER_TABLE[]
    = { &BEA_GPIO_DRIVER, &BEA_SPI_DRIVER, &BEA_SD_DRIVER, NULL };
