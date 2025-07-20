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
  for (;;)
    {
      BEA_DRIVER_TABLE[3]->request (NULL, NULL);
      bea_log (BEA_LOG_DEBUG, "Is it on???");
    }
  struct bea_sd_request_arg rq = {
    .type = BEA_SD_IS_PRESENT,
  };
  struct bea_sd_request_response resp;
  BEA_DRIVER_TABLE[2]->request (&rq, &resp);
  if (resp.succeeded && resp.is_present)
    {
      bea_log (BEA_LOG_INFO, "SD card is present");
      rq.type = BEA_SD_ACTIVATE;
      BEA_DRIVER_TABLE[2]->request (&rq, &resp);
      bea_log (BEA_LOG_INFO, "SD card enabled");
    }
  for (;;)
    {
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
