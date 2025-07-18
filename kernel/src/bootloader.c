#include <bootloader.h>
#include <driver_table.h>
#include <logging.h>
#include <sd/sd.h>

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
  struct bea_sd_request_arg rq = {
    .type = BEA_SD_IS_PRESENT,
  };
  struct bea_sd_request_response resp;
  for (;;)
    {
      BEA_DRIVER_TABLE[2]->request ((void *)&rq, (void *)&resp);
      if (resp.is_present && resp.succeeded)
        {
          bea_log (BEA_LOG_INFO, "SD card is present");
        }
      else if (!resp.succeeded)
        {
          bea_log (BEA_LOG_ERROR, "SD polling failed");
        }
      else
        {
          bea_log (BEA_LOG_INFO, "SD card is not present");
        }
    }
}
