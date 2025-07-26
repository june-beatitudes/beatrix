#include <driver.h>
#include <gpio/gpio.h>
#include <logging.h>
#include <rcc.h>
#include <register_utils.h>
#include <spi/spi.h>
#include <stdbool.h>

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
bea_spi_initialize (void)
{
  bea_rcc_enable_peripheral (BEA_APB2, 12);
  bea_rcc_enable_peripheral (BEA_APB1, 14);
  return true;
}

bool
bea_spi_deinitialize (void)
{
  // TODO: come back to this...
  return true;
}

static void
do_spi_channel_gpio_init (struct bea_spi_channel_config cfg)
{
  struct bea_gpio_request_arg mode_arg = { .type = BEA_GPIO_SET_MODE, .mode = BEA_GPIO_ALT };
  struct bea_gpio_request_arg altfunc_arg = { .type = BEA_GPIO_SET_ALT_FUNC, .alt_func = 5 };
  struct bea_gpio_request_response gpio_response;
  // Main in, subordinate out
  mode_arg.line = altfunc_arg.line = BEA_SPI_D_MISO;
  bea_set_reg_bits ((uint32_t *)BEA_SPI_D_MISO.bank + 0x02, 5, 4, 0b11);
  bea_gpio_request (&mode_arg, &gpio_response);
  bea_gpio_request (&altfunc_arg, &gpio_response);
  // Main out, subordinate in
  mode_arg.line = altfunc_arg.line = BEA_SPI_D_MOSI;
  bea_set_reg_bits ((uint32_t *)BEA_SPI_D_MOSI.bank + 0x02, 31, 30, 0b11);
  bea_gpio_request (&mode_arg, &gpio_response);
  bea_gpio_request (&altfunc_arg, &gpio_response);
  // Clock
  mode_arg.line = altfunc_arg.line = BEA_SPI_D_SCK;
  bea_set_reg_bits ((uint32_t *)BEA_SPI_D_SCK.bank + 0x02, 3, 2, 0b11);
  bea_gpio_request (&mode_arg, &gpio_response);
  bea_gpio_request (&altfunc_arg, &gpio_response);
}

static void
do_spi_channel_init (struct bea_spi_channel_config cfg)
{
  uint32_t cr1 = 0x0;
  bea_set_reg_bits (&cr1, 13, 13, 0);
  bea_set_reg_bits (&cr1, 5, 3, 0b011);
  bea_set_reg_bits (&cr1, 1, 0, 0b00);
  bea_set_reg_bits (&cr1, 9, 9, 1);
  bea_set_reg_bits (&cr1, 8, 8, 0);
  switch (cfg.comm_mode)
    {
    case BEA_SPI_FULL_DUPLEX:
      bea_set_reg_bits (&cr1, 15, 15, 0);
      bea_set_reg_bits (&cr1, 10, 10, 0);
      break;
    case BEA_SPI_HALF_DUPLEX_OUT_ENABLED:
      bea_set_reg_bits (&cr1, 15, 14, 0b11);
      break;
    case BEA_SPI_HALF_DUPLEX_OUT_DISABLED:
      bea_set_reg_bits (&cr1, 15, 14, 0b10);
      break;
    case BEA_SPI_SIMPLEX:
      bea_set_reg_bits (&cr1, 10, 10, 1);
      break;
    }
  bea_set_reg_bits (&cr1, 7, 7, 0);
  bea_set_reg_bits (&cr1, 2, 2, 0b1);
  bea_set_reg_bits ((uint32_t *)(cfg.chan) + 0x01, 12, 8, 0b0111);
  bea_set_reg_bits ((uint32_t *)(cfg.chan) + 0x01, 2, 2, 0b1);
  bea_set_reg_bits ((uint32_t *)(cfg.chan), 15, 0, cr1);
}

void
bea_spi_request (void *arg, void *result)
{
  struct bea_spi_request_arg typed = *((struct bea_spi_request_arg *)arg);
  struct bea_spi_request_response resp;

  switch (typed.type)
    {
    case BEA_SPI_INIT_CHANNEL:
      do_spi_channel_gpio_init (typed.channel_cfg);
      do_spi_channel_init (typed.channel_cfg);
    case BEA_SPI_DEINIT_CHANNEL:
      break;
    case BEA_SPI_TXRX:
      bea_set_reg_bits ((uint32_t *)(typed.channel_cfg.chan), 6, 6, 0b1);
      while (!bea_get_reg_bits ((uint32_t *)(typed.channel_cfg.chan) + 0x02, 1, 1))
        ;
      *((uint8_t *)(typed.channel_cfg.chan) + 0x0C) = typed.packet;
      if (!typed.ignore_resp)
        {
          while (!bea_get_reg_bits ((uint32_t *)(typed.channel_cfg.chan) + 0x02, 0, 0))
            ;
          resp.packet = *((uint8_t *)(typed.channel_cfg.chan) + 0x0C);
        }
      bea_set_reg_bits ((uint32_t *)(typed.channel_cfg.chan), 6, 6, 0b0);
      break;
    }

  resp.succeeded = true;
  *((struct bea_spi_request_response *)result) = resp;
}

const struct bea_driver BEA_SPI_DRIVER = {
  .name = "spi",
  .initialize = bea_spi_initialize,
  .deinitialize = bea_spi_deinitialize,
  .request = bea_spi_request,
};
