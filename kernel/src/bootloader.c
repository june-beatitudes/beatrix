#include <bootloader.h>
#include <driver_table.h>
#include <graphics/graphics.h>
#include <logging.h>
#include <sd/sd.h>
#include <stdint.h>

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
  struct bea_sd_request_arg rq = {
    .type = BEA_SD_IS_PRESENT,
  };
  struct bea_sd_request_response resp;
  BEA_DRIVER_TABLE[2]->request (&rq, &resp);
  if ((resp.err == BEA_SD_ERROR_NONE) && resp.is_present)
    {
      bea_log (BEA_LOG_INFO, "SD card is present");
      rq.type = BEA_SD_ACTIVATE;
      BEA_DRIVER_TABLE[2]->request (&rq, &resp);
      if (resp.err == BEA_SD_ERROR_NONE)
        {
          bea_log (BEA_LOG_INFO, "SD card enabled");
          rq.type = BEA_SD_COUNT_BLOCKS;
          BEA_DRIVER_TABLE[2]->request (&rq, &resp);
          __attribute__ ((unused)) uint32_t n_blocks = resp.block_count;
          bea_log (BEA_LOG_INFO, "SD blocks counted");
          uint8_t buf[512 * 10];
          rq = (struct bea_sd_request_arg){
            .type = BEA_SD_READ_BLOCKS,
            .n_blocks = 10,
            .block_addr = 0,
            .buffer = buf,
          };
          BEA_DRIVER_TABLE[2]->request (&rq, &resp);
          bea_log (BEA_LOG_INFO, "SD block 1 read");
        }
      else
        {
          bea_log (BEA_LOG_ERROR, "SD card enable failed");
        }
    }
  for (;;)
    {
      bea_log (BEA_LOG_INFO, "Busy looping");
    }
}
