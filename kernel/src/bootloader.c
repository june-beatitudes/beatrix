#include <bootloader.h>
#include <driver_table.h>
#include <gpio/gpio.h>
#include <logging.h>

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
  struct bea_gpio_request_arg arg1 = {
    .type = BEA_GPIO_SET_MODE,
    .mode = BEA_GPIO_PULLUP,
    .pin = 10,
    .bank = BEA_GPIO_BANK_B,
  };
  struct bea_gpio_request_arg arg2 = {
    .type = BEA_GPIO_READ_VALUE,
    .pin = 10,
    .bank = BEA_GPIO_BANK_B,
  };
  struct bea_gpio_request_response val;
  BEA_DRIVER_TABLE[0]->request ((void *)&arg1, (void *)&val);
  for (;;)
    {
      BEA_DRIVER_TABLE[0]->request ((void *)&arg2, (void *)&val);
      if (val.value)
        {
          bea_log (BEA_LOG_INFO, "Button not pushed");
        }
      else
        {
          bea_log (BEA_LOG_INFO, "Button pushed");
        }
    }
}
