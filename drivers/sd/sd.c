#include <gpio/gpio.h>
#include <sd/sd.h>
#include <spi/spi.h>

const struct bea_gpio_line BEA_SD_CD = {
  .bank = BEA_GPIO_BANK_C,
  .pin = 10,
};

const struct bea_gpio_line BEA_SD_CS = {
  .bank = BEA_GPIO_BANK_C,
  .pin = 12,
};

const struct bea_gpio_line BEA_SPI_D_MISO = {
  .bank = BEA_GPIO_BANK_C,
  .pin = 2,
};

const struct bea_gpio_line BEA_SPI_D_MOSI = {
  .bank = BEA_GPIO_BANK_B,
  .pin = 15,
};

const struct bea_gpio_line BEA_SPI_D_SCK = {
  .bank = BEA_GPIO_BANK_D,
  .pin = 1,
};

bool
bea_sd_initialize (void)
{
  struct bea_gpio_request_arg mode_arg;
  struct bea_gpio_request_arg altfunc_arg;
  struct bea_gpio_request_response gpio_response;
  mode_arg.type = BEA_GPIO_SET_MODE;

  // Card detect pin
  mode_arg.mode = BEA_GPIO_PULLUP;
  mode_arg.line = BEA_SD_CD;
  bea_gpio_request (&mode_arg, &gpio_response);

  // Chip select
  mode_arg.mode = BEA_GPIO_OUT;
  mode_arg.line = BEA_SD_CS;
  bea_gpio_request (&mode_arg, &gpio_response);

  // Main in, subordinate out
  mode_arg.mode = BEA_GPIO_ALT;
  altfunc_arg.alt_func = 1;
  mode_arg.line = altfunc_arg.line = BEA_SPI_D_MISO;
  bea_gpio_request (&mode_arg, &gpio_response);
  bea_gpio_request (&altfunc_arg, &gpio_response);

  // Main out, subordinate in
  altfunc_arg.alt_func = 2;
  mode_arg.line = altfunc_arg.line = BEA_SPI_D_MOSI;
  bea_gpio_request (&mode_arg, &gpio_response);
  bea_gpio_request (&altfunc_arg, &gpio_response);

  // Clock
  altfunc_arg.alt_func = 1;
  mode_arg.line = altfunc_arg.line = BEA_SPI_D_SCK;
  bea_gpio_request (&mode_arg, &gpio_response);
  bea_gpio_request (&altfunc_arg, &gpio_response);

  return true;
}

bool
bea_sd_deinitialize (void)
{
  // Do nothing
  return true;
}

void
bea_sd_request (void *arg, void *out)
{
  struct bea_sd_request_arg tin = *((struct bea_sd_request_arg *)arg);
  struct bea_sd_request_response *tout = (struct bea_sd_request_response *)out;

  struct bea_gpio_request_response resp;
  struct bea_gpio_request_arg gpio_rq = {
    .type = BEA_GPIO_READ_VALUE,
    .line = BEA_SD_CD,
  }; // Used only for checking if SD is present
  switch (tin.type)
    {
    case BEA_SD_IS_PRESENT:
      bea_gpio_request (&gpio_rq, &resp);
      tout->succeeded = resp.succeeded;
      tout->is_present = !resp.value;
      break;
    default:
      break;
    }
}

const struct bea_driver BEA_SD_DRIVER = {
  .name = "sd",
  .initialize = bea_sd_initialize,
  .deinitialize = bea_sd_deinitialize,
  .request = bea_sd_request,
};
