#include <bootloader.h>
#include <driver_table.h>
#include <ff.h>
#include <graphics/graphics.h>
#include <logging.h>
#include <sd/sd.h>
#include <stdint.h>
#include <shell_main.h>

extern const uint8_t bea_logo[512];

void
bea_do_boot (void)
{
  for (size_t i = 0; BEA_DRIVER_TABLE[i]->name[0] != '\0'; ++i)
    {
      bea_log (BEA_LOG_INFO, BEA_DRIVER_TABLE[i]->name);
      if (!BEA_DRIVER_TABLE[i]->initialize ())
        {
          bea_log (BEA_LOG_ERROR, "Driver initialization failed!");
        }
      else
        {
          bea_log (BEA_LOG_INFO, "Driver initialization successful");
        }
    }
  struct bea_graphics_request_arg drawrq = {
    .type = BEA_GRAPHICS_DRAW_BMP,
    .x = 31,
    .y = 0,
    .buf = bea_logo,
    .width = 64,
    .height = 64,
  };
  struct bea_graphics_request_response _;
  BEA_DRIVER_TABLE[4]->request (&drawrq, &_);
  struct bea_graphics_request_arg updaterq = {
    .type = BEA_GRAPHICS_UPDATE_DISPLAY,
  };
  BEA_DRIVER_TABLE[4]->request (&updaterq, &_);
  shell_main ();
}
