#include <driver.h>
#include <gpio/gpio.h>
#include <rcc.h>
#include <register_utils.h>
#include <spi/spi.h>
#include <stdbool.h>

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
do_spi_channel_init (struct bea_spi_channel_config cfg)
{
  bea_set_reg_bits ((uint32_t *)(cfg.chan), 5, 3, 0b000);
  switch (cfg.comm_mode)
    {
    case BEA_SPI_FULL_DUPLEX:
      bea_set_reg_bits ((uint32_t *)(cfg.chan), 15, 15, 0);
      bea_set_reg_bits ((uint32_t *)(cfg.chan), 10, 10, 0);
      break;
    case BEA_SPI_HALF_DUPLEX_OUT_ENABLED:
      bea_set_reg_bits ((uint32_t *)(cfg.chan), 15, 14, 0b11);
      break;
    case BEA_SPI_HALF_DUPLEX_OUT_DISABLED:
      bea_set_reg_bits ((uint32_t *)(cfg.chan), 15, 14, 0b10);
      break;
    case BEA_SPI_SIMPLEX:
      bea_set_reg_bits ((uint32_t *)(cfg.chan), 10, 10, 1);
      break;
    }
  bea_set_reg_bits ((uint32_t *)(cfg.chan), 1, 0, 0b00);
  bea_set_reg_bits ((uint32_t *)(cfg.chan), 7, 7, 0);
  bea_set_reg_bits ((uint32_t *)(cfg.chan + 0x04), 11, 8, 0b0111);
  bea_set_reg_bits ((uint32_t *)(cfg.chan), 6, 6, 1);
}

void
bea_spi_request (void *arg, void *result)
{
  struct bea_spi_request_arg typed = *((struct bea_spi_request_arg *)arg);
  struct bea_spi_request_response resp;

  switch (typed.type)
    {
    case BEA_SPI_INIT_CHANNEL:
      do_spi_channel_init (typed.channel_cfg);
    case BEA_SPI_DEINIT_CHANNEL:
      break;
    case BEA_SPI_READ:
      while (
          bea_get_reg_bits ((uint32_t *)(typed.channel_cfg.chan + 0x08), 10, 9)
          == 0)
        ;
      resp.packet = bea_get_reg_bits (
          (uint32_t *)(typed.channel_cfg.chan + 0x0C), 7, 0);
      break;
    case BEA_SPI_WRITE:
      while (bea_get_reg_bits ((uint32_t *)(typed.channel_cfg.chan + 0x08), 12,
                               11)
             == 0b11)
        ;
      bea_set_reg_bits ((uint32_t *)(typed.channel_cfg.chan + 0x0C), 7, 0,
                        typed.packet);
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
