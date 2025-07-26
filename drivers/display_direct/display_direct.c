
#include <display_direct/display_direct.h>
#include <gpio/gpio.h>
#include <logging.h>
#include <register_utils.h>
#include <spi/spi.h>

const struct bea_gpio_line BEA_LCD_CS = {
  .bank = BEA_GPIO_BANK_C,
  .pin = 11,
};

const struct bea_gpio_line BEA_LCD_RST = {
  .bank = BEA_GPIO_BANK_B,
  .pin = 0,
};

const struct bea_gpio_line BEA_LCD_DI = {
  .bank = BEA_GPIO_BANK_B,
  .pin = 1,
};

static void
lcd_send_cmd (uint8_t cmd)
{
  struct bea_gpio_request_arg gpio_rq = {
    .type = BEA_GPIO_SET_VALUE,
    .line = BEA_LCD_CS,
    .value = false,
  };
  struct bea_gpio_request_response gpio_resp;
  bea_gpio_request (&gpio_rq, &gpio_resp);
  gpio_rq.line = BEA_LCD_DI;
  bea_gpio_request (&gpio_rq, &gpio_resp);
  struct bea_spi_request_arg spi_rq = {
		.type = BEA_SPI_TXRX,
		.channel_cfg = {
			.chan = BEA_SPI_CHAN2,
			.comm_mode = BEA_SPI_FULL_DUPLEX,
		},
		.packet = cmd,
		.ignore_resp = true,
	};
  struct bea_spi_request_response spi_resp;
  bea_spi_request (&spi_rq, &spi_resp);
  gpio_rq.value = true;
  gpio_rq.line = BEA_LCD_CS;
  bea_gpio_request (&gpio_rq, &gpio_resp);
}

static void
lcd_send_data_packet (uint8_t packet)
{
  struct bea_gpio_request_arg gpio_rq = {
    .type = BEA_GPIO_SET_VALUE,
    .line = BEA_LCD_CS,
    .value = false,
  };
  struct bea_gpio_request_response gpio_resp;
  bea_gpio_request (&gpio_rq, &gpio_resp);
  gpio_rq.value = true;
  gpio_rq.line = BEA_LCD_DI;
  bea_gpio_request (&gpio_rq, &gpio_resp);
  struct bea_spi_request_arg spi_rq = {
		.type = BEA_SPI_TXRX,
		.channel_cfg = {
			.chan = BEA_SPI_CHAN2,
			.comm_mode = BEA_SPI_FULL_DUPLEX,
		},
		.packet = packet,
		.ignore_resp = true,
	};
  struct bea_spi_request_response spi_resp;
  bea_spi_request (&spi_rq, &spi_resp);
  gpio_rq.line = BEA_LCD_CS;
  bea_gpio_request (&gpio_rq, &gpio_resp);
}

const static uint8_t LCD_INIT_COMMAND_SEQ[]
    = { 0xE2, 0xA2, 0xA0, 0xC0, 0x24, 0x81, 0x30, 0x2C, 0x2E, 0x2F, 0xA7 };

bool
bea_display_direct_initialize (void)
{
  struct bea_gpio_request_arg gpio_rq = {
    .type = BEA_GPIO_SET_MODE,
    .line = BEA_LCD_CS,
    .mode = BEA_GPIO_OUT,
  };
  struct bea_gpio_request_response gpio_resp;
  bea_gpio_request (&gpio_rq, &gpio_resp);
  bea_set_reg_bits ((uint32_t *)gpio_rq.line.bank + 0x3, gpio_rq.line.pin * 2 + 1,
                    gpio_rq.line.pin * 2, 0b01);
  gpio_rq.line = BEA_LCD_RST;
  bea_gpio_request (&gpio_rq, &gpio_resp);
  bea_set_reg_bits ((uint32_t *)gpio_rq.line.bank + 0x3, gpio_rq.line.pin * 2 + 1,
                    gpio_rq.line.pin * 2, 0b01);
  gpio_rq.line = BEA_LCD_DI;
  bea_gpio_request (&gpio_rq, &gpio_resp);
  bea_set_reg_bits ((uint32_t *)gpio_rq.line.bank + 0x3, gpio_rq.line.pin * 2 + 1,
                    gpio_rq.line.pin * 2, 0b01);

  gpio_rq.type = BEA_GPIO_SET_VALUE;
  gpio_rq.line = BEA_LCD_RST;
  gpio_rq.value = false;
  bea_gpio_request (&gpio_rq, &gpio_resp);
  bea_log (BEA_LOG_DEBUG, "Stand-in for a delay");
  gpio_rq.value = true;
  bea_gpio_request (&gpio_rq, &gpio_resp);

  struct bea_spi_request_arg spi_rq = {
		.type = BEA_SPI_INIT_CHANNEL,
		.channel_cfg = {
			.chan = BEA_SPI_CHAN2,
			.comm_mode = BEA_SPI_FULL_DUPLEX,
		},
	};
  struct bea_spi_request_response spi_resp;
  bea_spi_request (&spi_rq, &spi_resp);

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
bea_display_direct_request (void *request, void *result)
{
  struct bea_display_direct_request_arg arg = *((struct bea_display_direct_request_arg *)request);
  lcd_send_cmd (0x40);
  for (size_t page = 0; page < 8; ++page)
    {
      lcd_send_cmd (0xB0 + page);
      lcd_send_cmd (0x10);
      lcd_send_cmd (0x00);
      for (size_t col = 0; col < 128; ++col)
        {
          lcd_send_data_packet (arg.framebuffer[col * 8 + page]);
        }
    }
  lcd_send_cmd (0xAF);
}

const struct bea_driver BEA_DISPLAY_DIRECT_DRIVER = {
  .name = "display_direct",
  .initialize = bea_display_direct_initialize,
  .deinitialize = bea_display_direct_deinitialize,
  .request = bea_display_direct_request,
};
