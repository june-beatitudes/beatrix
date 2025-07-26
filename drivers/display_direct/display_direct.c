
#include <display_direct/display_direct.h>
#include <gpio/gpio.h>
#include <logging.h>
#include <register_utils.h>
#include <spi/spi.h>

const struct bea_gpio_line DD_CHIP_SELECT = {
  .bank = BEA_GPIO_BANK_C,
  .pin = 11,
};

const struct bea_gpio_line DD_RESET = {
  .bank = BEA_GPIO_BANK_B,
  .pin = 0,
};

const struct bea_gpio_line DD_DATA_INSTRUCTION = {
  .bank = BEA_GPIO_BANK_B,
  .pin = 1,
};

const enum bea_spi_channel DD_SPI_CHANNEL = BEA_SPI_CHAN2;

static void
lcd_send_cmd (uint8_t cmd)
{
  bea_gpio_set_value (DD_CHIP_SELECT, false);
  bea_gpio_set_value (DD_DATA_INSTRUCTION, false);
  // TODO: Do something about this!
  enum bea_spi_error spi_err
      = bea_spi_txrx_blocking (DD_SPI_CHANNEL, &cmd, NULL, 1);
  bea_gpio_set_value (DD_CHIP_SELECT, true);
}

static void
lcd_send_data_packet (uint8_t packet)
{
  bea_gpio_set_value (DD_CHIP_SELECT, false);
  bea_gpio_set_value (DD_DATA_INSTRUCTION, true);
  // TODO: Do something about this!
  enum bea_spi_error spi_err
      = bea_spi_txrx_blocking (DD_SPI_CHANNEL, &packet, NULL, 1);
  bea_gpio_set_value (DD_CHIP_SELECT, true);
}

const static uint8_t LCD_INIT_COMMAND_SEQ[]
    = { 0xE2, 0xA2, 0xA0, 0xC0, 0x24, 0x81, 0x30, 0x2C, 0x2E, 0x2F, 0xA7 };

bool
bea_display_direct_initialize (void)
{
  bea_gpio_set_mode (DD_CHIP_SELECT, BEA_GPIO_OUT);
  bea_gpio_set_pulldir (DD_CHIP_SELECT, BEA_GPIO_DIR_PULLUP);
  bea_gpio_set_value (DD_CHIP_SELECT, true);
  bea_gpio_set_mode (DD_RESET, BEA_GPIO_OUT);
  bea_gpio_set_pulldir (DD_RESET, BEA_GPIO_DIR_PULLUP);
  bea_gpio_set_mode (DD_DATA_INSTRUCTION, BEA_GPIO_OUT);
  bea_gpio_set_pulldir (DD_DATA_INSTRUCTION, BEA_GPIO_DIR_PULLUP);

  bea_gpio_set_value (DD_RESET, false);
  bea_log (BEA_LOG_DEBUG, "Stand-in for a delay");
  bea_gpio_set_value (DD_RESET, true);

  for (size_t i = 0; i < sizeof (LCD_INIT_COMMAND_SEQ); ++i)
    {
      lcd_send_cmd (LCD_INIT_COMMAND_SEQ[i]);
    }

  return true;
}

bool
bea_display_direct_deinitialize (void)
{
  return true;
}

void
bea_display_direct_render (const uint8_t *framebuffer)
{
  lcd_send_cmd (0x40);
  for (size_t page = 0; page < 8; ++page)
    {
      lcd_send_cmd (0xB0 + page);
      lcd_send_cmd (0x10);
      lcd_send_cmd (0x00);
      for (size_t col = 0; col < 128; ++col)
        {
          lcd_send_data_packet (framebuffer[col * 8 + page]);
        }
    }
  lcd_send_cmd (0xAF);
}

void
bea_display_direct_request (void *request, void *result)
{
  struct bea_display_direct_request_arg arg
      = *((struct bea_display_direct_request_arg *)request);
  bea_display_direct_render (arg.framebuffer);
}

const struct bea_driver BEA_DISPLAY_DIRECT_DRIVER = {
  .name = "display_direct",
  .initialize = bea_display_direct_initialize,
  .deinitialize = bea_display_direct_deinitialize,
  .request = bea_display_direct_request,
};
