#include <gpio/gpio.h>
#include <logging.h>
#include <register_utils.h>
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

const static struct bea_spi_channel_config sd_spi_channel = {
  .chan = BEA_SPI_CHAN2,
  .comm_mode = BEA_SPI_FULL_DUPLEX,
};

/// Set during card enable (some are byte addressed, some block)
static bool sd_is_block_addressed;

enum bea_sd_cmd_response_class
{
  BEA_SD_R1,
  BEA_SD_R3,
  BEA_SD_R7,
};

struct bea_sd_cmd_response
{
  enum bea_sd_cmd_response_class class;
  uint8_t r1;
  uint32_t trailing;
};

bool
bea_sd_initialize (void)
{
  struct bea_spi_request_arg init_arg = {
    .type = BEA_SPI_INIT_CHANNEL,
    .channel_cfg = sd_spi_channel,
  };
  struct bea_spi_request_response _;
  bea_spi_request (&init_arg, &_);

  struct bea_gpio_request_arg mode_arg;
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
  struct bea_gpio_request_arg cs_arg
      = { .type = BEA_GPIO_SET_VALUE, .line = BEA_SD_CS, .value = true };
  bea_gpio_request (&cs_arg, &gpio_response);
  bea_set_reg_bits ((uint32_t *)BEA_SD_CS.bank + 0x02, 25, 24, 0b11);
  bea_set_reg_bits ((uint32_t *)BEA_SD_CS.bank + 0x03, 25, 24, 0b01);

  return true;
}

static void
do_real_spi_txrx (const uint8_t *buf, size_t n, bool is_tx)
{
  struct bea_spi_request_arg rq = {
    .type = BEA_SPI_WRITE,
    .channel_cfg = sd_spi_channel,
  };
  struct bea_spi_request_response _;
  for (size_t i = 0; i < n; ++i)
    {
      rq.packet = buf[i];
      bea_spi_request (&rq, &_);
    }
  /*uint32_t channel_config = 0x0;

  bea_set_reg_bits ((uint32_t *)0x40020014, 31, 0, (uint32_t)buf);
  bea_set_reg_bits ((uint32_t *)0x40020010, 31, 0, 0x4000380C);
  bea_set_reg_bits ((uint32_t *)0x4002000C, 15, 0, n);
  bea_set_reg_bits (&channel_config, 13, 12, 0b11);
  bea_set_reg_bits (&channel_config, 11, 10, 0b00);
  bea_set_reg_bits (&channel_config, 9, 8, 0b01);
  bea_set_reg_bits (&channel_config, 7, 7, 1);
  bea_set_reg_bits (&channel_config, 6, 6, 1);
  bea_set_reg_bits (&channel_config, 4, 4, (is_tx) ? 1 : 0);
  bea_set_reg_bits (&channel_config, 0, 0, 1);
  bea_set_reg_bits ((uint32_t *)0x40020008, 31, 0, channel_config);*/
}

__attribute__ ((warn_unused_result)) static struct bea_sd_cmd_response
sd_send_cmd (uint8_t cmd_index, uint32_t arg)
{
  struct bea_spi_request_arg rq = {
    .type = BEA_SPI_WRITE,
    .channel_cfg = sd_spi_channel,
    .packet = cmd_index | 0x40,
  };
  struct bea_spi_request_response resp;
  bea_spi_request (&rq, &resp);
  for (size_t i = 4; i > 0; --i)
    {
      rq.packet = 0x0;
      bea_spi_request (&rq, &resp);
    }
  rq.packet = 0x95;
  bea_spi_request (&rq, &resp);
  // bea_set_reg_bits ((uint32_t *)sd_spi_channel.chan, 12, 12, 1);
  struct bea_sd_cmd_response ret = {
    .r1 = resp.packet,
  };
  if (cmd_index == 58 || cmd_index == 8)
    {
      for (size_t i = 4; i > 0; --i)
        {
          ((uint8_t *)ret.trailing)[i - 1] = resp.packet;
          bea_spi_request (&rq, &resp);
        }
      ret.class = (cmd_index == 58) ? BEA_SD_R3 : BEA_SD_R7;
    }
  else
    {
      ret.class = BEA_SD_R1;
    }
  return ret;
}

static void
sd_enable (uint32_t blk_length)
{
  struct bea_gpio_request_response _;
  struct bea_gpio_request_arg gpio_rq = {
    .type = BEA_GPIO_SET_VALUE,
    .line = BEA_SD_CS,
    .value = true,
  };
  bea_gpio_request (&gpio_rq, &_);
  struct bea_spi_request_arg spi_rq = {
    .type = BEA_SPI_WRITE,
    .channel_cfg = sd_spi_channel,
    .packet = 0xFF,
  };
  struct bea_spi_request_response spi_resp;
  for (size_t i = 0; i < 10; ++i)
    {
      bea_spi_request (&spi_rq, &spi_resp);
    }
  gpio_rq.value = false;
  bea_gpio_request (&gpio_rq, &_);
  const uint8_t PACKETS[6] = { 0x40, 0x0, 0x0, 0x0, 0x0, 0x95 };
  do_real_spi_txrx (PACKETS, sizeof (PACKETS), true);

  spi_rq.packet = 0xFF;
  do
    {
      spi_rq.type = BEA_SPI_READ;
      bea_spi_request (&spi_rq, &spi_resp);
      spi_rq.type = BEA_SPI_WRITE;
      bea_spi_request (&spi_rq, &spi_resp);
    }
  while (spi_resp.packet == 0xFF);
  bea_log (BEA_LOG_DEBUG, "Made it here!");
  gpio_rq.value = true;
  bea_gpio_request (&gpio_rq, &_);
  bea_spi_request (&spi_rq, &spi_resp);
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
    case BEA_SD_ACTIVATE:
      sd_enable (512);
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
