#include <driver.h>
#include <gpio/gpio.h>
#include <logging.h>
#include <rcc.h>
#include <register_utils.h>
#include <spi/spi.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

const struct bea_gpio_line SPI1_MISO = {
  .bank = BEA_GPIO_BANK_B,
  .pin = 4,
};

const struct bea_gpio_line SPI1_MOSI = {
  .bank = BEA_GPIO_BANK_B,
  .pin = 5,
};

const struct bea_gpio_line SPI1_SCK = {
  .bank = BEA_GPIO_BANK_A,
  .pin = 5,
};

const struct bea_gpio_line SPI2_MISO = {
  .bank = BEA_GPIO_BANK_C,
  .pin = 2,
};

const struct bea_gpio_line SPI2_MOSI = {
  .bank = BEA_GPIO_BANK_B,
  .pin = 15,
};

const struct bea_gpio_line SPI2_SCK = {
  .bank = BEA_GPIO_BANK_D,
  .pin = 1,
};

enum spi_register_offset : uint32_t
{
  SPI_REG_CR1 = 0x0,
  SPI_REG_CR2 = 0x1,
  SPI_REG_SR = 0x2,
  SPI_REG_DR = 0x3,
  SPI_REG_CRCPR = 0x4,
  SPI_REG_RXCRCR = 0x5,
  SPI_REG_TXCRCR = 0x6,
};

static void
do_spi_channel_gpio_inits ()
{
  const struct bea_gpio_line LINES[]
      = { SPI1_MISO, SPI1_MOSI, SPI1_SCK, SPI2_MISO, SPI2_MOSI, SPI2_SCK };
  for (size_t i = 0; i < sizeof (LINES); ++i)
    {
      bea_gpio_set_mode (LINES[i], BEA_GPIO_ALT);
      bea_gpio_set_alt_func (LINES[i], BEA_GPIO_AF_SPI1_2);
    }
}

static void
do_spi_channel_inits ()
{
  const enum bea_spi_channel CHANNELS[] = { BEA_SPI_CHAN1, BEA_SPI_CHAN2 };
  for (size_t i = 0; i < sizeof (CHANNELS); ++i)
    {
      uint32_t cr1 = 0x0;
      // TODO: Holy magic constants, Batman!
      bea_set_reg_bits (&cr1, 15, 15, 0);
      bea_set_reg_bits (&cr1, 13, 13, 0);
      bea_set_reg_bits (&cr1, 10, 10, 0);
      bea_set_reg_bits (&cr1, 9, 9, 1);
      bea_set_reg_bits (&cr1, 8, 8, 0);
      bea_set_reg_bits (&cr1, 7, 7, 0);
      bea_set_reg_bits (&cr1, 6, 6, 0);
      bea_set_reg_bits (&cr1, 5, 3, 0b011);
      bea_set_reg_bits (&cr1, 2, 2, 0b1);
      bea_set_reg_bits (&cr1, 1, 0, 0b00);
      bea_set_reg_bits ((uint32_t *)CHANNELS[i] + SPI_REG_CR2, 12, 8, 0b10111);
      bea_set_reg_bits ((uint32_t *)CHANNELS[i] + SPI_REG_CR2, 2, 2, 0b1);
      bea_set_reg_bits ((uint32_t *)CHANNELS[i] + SPI_REG_CR1, 15, 0, cr1);
    }
}

bool
bea_spi_initialize (void)
{
  bea_rcc_enable_peripheral (BEA_APB2, 12);
  bea_rcc_enable_peripheral (BEA_APB1, 14);
  do_spi_channel_gpio_inits ();
  do_spi_channel_inits ();
  return true;
}

bool
bea_spi_deinitialize (void)
{
  // TODO: come back to this...
  return true;
}

__attribute__ ((warn_unused_result)) enum bea_spi_error
bea_spi_txrx_blocking (enum bea_spi_channel chan, uint8_t *tx, uint8_t *rx,
                       size_t n)
{
  // Enable SPI
  bea_set_reg_bits ((uint32_t *)(chan) + SPI_REG_CR1, 6, 6, 0b1);

  for (size_t i = 0; i < n; ++i)
    {
      // Wait for TX to be free, then send (if we want to)
      // TODO: Add timeout
      while (!bea_get_reg_bits ((uint32_t *)(chan) + SPI_REG_SR, 1, 1))
        ;
      *((uint8_t *)(chan) + 4 * SPI_REG_DR) = tx[i];
      if (rx != NULL)
        {
          while (!bea_get_reg_bits ((uint32_t *)(chan) + SPI_REG_SR, 0, 0))
            ;
          rx[i] = *((uint8_t *)(chan) + 4 * SPI_REG_DR);
        }
    }

  // Disable SPI
  bea_set_reg_bits ((uint32_t *)(chan) + SPI_REG_CR1, 6, 6, 0b0);

  return BEA_SPI_ERROR_NONE;
}

void
bea_spi_enable_crc (enum bea_spi_channel chan, bool is_crc8)
{
  bea_set_reg_bits ((uint32_t *)chan + SPI_REG_CR1, 13, 13, 0b1);
  if (is_crc8)
    {
      bea_set_reg_bits ((uint32_t *)chan + SPI_REG_CR1, 11, 11, 0);
      bea_set_reg_bits ((uint32_t *)chan + SPI_REG_CRCPR, 15, 0, 0x9);
    }
  else
    {
      bea_set_reg_bits ((uint32_t *)chan + SPI_REG_CR1, 11, 11, 0b1);
    }
}

void
bea_spi_disable_crc (enum bea_spi_channel chan)
{
  bea_set_reg_bits ((uint32_t *)chan + SPI_REG_CR1, 13, 13, 0b0);
}

__attribute__ ((warn_unused_result)) uint16_t
bea_spi_get_txcrc (enum bea_spi_channel chan)
{
  return bea_get_reg_bits ((uint32_t *)chan + SPI_REG_TXCRCR, 15, 0);
}

__attribute__ ((warn_unused_result)) uint16_t
bea_spi_get_rxcrc (enum bea_spi_channel chan)
{
  return bea_get_reg_bits ((uint32_t *)chan + SPI_REG_RXCRCR, 15, 0);
}

void
bea_spi_request (void *arg, void *result)
{
  struct bea_spi_request_arg typed = *((struct bea_spi_request_arg *)arg);
  struct bea_spi_request_response resp;

  switch (typed.type)
    {
    case BEA_SPI_TXRX:
      resp.err = bea_spi_txrx_blocking (typed.channel, typed.tx_buf,
                                        typed.rx_buf, typed.n_bytes);
      break;
    case BEA_SPI_ENABLE_CRC:
      bea_spi_enable_crc (typed.channel, typed.is_crc8);
      resp.err = BEA_SPI_ERROR_NONE;
      break;
    case BEA_SPI_DISABLE_CRC:
      bea_spi_disable_crc (typed.channel);
      resp.err = BEA_SPI_ERROR_NONE;
    case BEA_SPI_GET_RXCRC:
      resp.crc = bea_spi_get_rxcrc (typed.channel);
      resp.err = BEA_SPI_ERROR_NONE;
      break;
    case BEA_SPI_GET_TXCRC:
      resp.crc = bea_spi_get_txcrc (typed.channel);
      resp.err = BEA_SPI_ERROR_NONE;
      break;
    }
  *((struct bea_spi_request_response *)result) = resp;
}

const struct bea_driver BEA_SPI_DRIVER = {
  .name = "spi",
  .initialize = bea_spi_initialize,
  .deinitialize = bea_spi_deinitialize,
  .request = bea_spi_request,
};
