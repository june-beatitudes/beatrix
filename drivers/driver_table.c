#include <display_direct/display_direct.h>
#include <driver_table.h>
#include <gpio/gpio.h>
#include <graphics/graphics.h>
#include <mem_utils.h>
#include <sd/sd.h>
#include <spi/spi.h>

const struct bea_driver *const BEA_DRIVER_TABLE[]
    = { &BEA_GPIO_DRIVER,           &BEA_SPI_DRIVER,      &BEA_SD_DRIVER,
        &BEA_DISPLAY_DIRECT_DRIVER, &BEA_GRAPHICS_DRIVER, NULL };

const struct bea_driver *const
bea_get_driver_by_name (const char *name)
{
  for (size_t i = 0; BEA_DRIVER_TABLE[i] != NULL; ++i)
    {
      if (strcmp (name, BEA_DRIVER_TABLE[i]->name) == 0)
        {
          return BEA_DRIVER_TABLE[i];
        }
    }
  return NULL;
}