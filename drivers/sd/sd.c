#include <gpio/gpio.h>
#include <logging.h>
#include <register_utils.h>
#include <sd/sd.h>
#include <spi/spi.h>
#include <stddef.h>

#define BLOCK_LENGTH_BYTES 512

const struct bea_gpio_line SD_CHIP_DETECT = {
  .bank = BEA_GPIO_BANK_C,
  .pin = 10,
};

const struct bea_gpio_line SD_CHIP_SELECT = {
  .bank = BEA_GPIO_BANK_C,
  .pin = 12,
};

const static enum bea_spi_channel SD_SPI_CHANNEL = BEA_SPI_CHAN2;

/// Set during card enable (some are byte addressed, some block)
static bool sd_is_block_addressed;

enum sd_cmd_response_class
{
  BEA_SD_R1,
  BEA_SD_R2,
  BEA_SD_R3,
  BEA_SD_R7,
};

struct sd_cmd_response
{
  enum bea_sd_error err;
  enum sd_cmd_response_class class;
  uint8_t r1;
  union
  {
    uint32_t r3_data;
    uint8_t r2_data[18];
  };
};

enum sd_r1_bit
{
  BEA_SD_R1_IDLE = 0b1,
  BEA_SD_R1_ERASE_RESET = 0b10,
  BEA_SD_R1_ILLEGAL_CMD = 0b100,
  BEA_SD_R1_CRC_ERR = 0b1000,
  BEA_SD_R1_ERASE_SEQ_ERR = 0b10000,
  BEA_SD_R1_ADDR_ERR = 0b100000,
  BEA_SD_R1_PARM_ERR = 0b1000000,
};

bool
bea_sd_initialize (void)
{
  // Card detect pin
  bea_gpio_set_mode (SD_CHIP_DETECT, BEA_GPIO_IN);
  bea_gpio_set_pulldir (SD_CHIP_DETECT, BEA_GPIO_DIR_PULLUP);

  // Chip select
  bea_gpio_set_mode (SD_CHIP_SELECT, BEA_GPIO_OUT);
  bea_gpio_set_pulldir (SD_CHIP_SELECT, BEA_GPIO_DIR_PULLUP);
  bea_gpio_set_value (SD_CHIP_SELECT, true);

  return true;
}

__attribute__ ((warn_unused_result)) static struct sd_cmd_response
sd_send_cmd (uint8_t cmd_index, uint32_t arg)
{
  uint8_t cmd_buf[5];
  cmd_buf[0] = 0x40 | cmd_index;
  for (size_t i = 1; i < 5; ++i)
    {
      cmd_buf[i] = 0xFF & (arg >> (8 * (4 - i)));
    }
  uint8_t ignore_buf[5]; // Can't just send NULL because we want to block until
                         // we've read/been acknowledged
  enum bea_spi_error spi_err
      = bea_spi_txrx_blocking (SD_SPI_CHANNEL, cmd_buf, ignore_buf, 5);
  uint8_t crc = 0xFF;
  switch (cmd_index)
    {
    case 0:
      crc = 0x95;
      break;
    case 8:
      crc = 0xC3;
      break;
    case 55:
      crc = 0x65;
      break;
    case 41:
      crc = (arg == 0) ? 0x57 : 0x61;
      break;
    }
  spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &crc, ignore_buf, 1);
  if (spi_err != BEA_SPI_ERROR_NONE)
    {
      return (struct sd_cmd_response){
        .err = BEA_SD_ERROR_WRITE,
      };
    }

  uint8_t filler_packet = 0xFF;
  uint8_t packet_response = 0xFF;
  while (packet_response == 0xFF)
    {
      spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &filler_packet,
                                       &packet_response, 1);
      if (spi_err != BEA_SPI_ERROR_NONE)
        {
          return (struct sd_cmd_response){
            .err = BEA_SD_ERROR_WRITE,
          };
        }
    }

  struct sd_cmd_response ret = {
    .r1 = packet_response,
    .r3_data = 0,
  };
  switch (cmd_index)
    {
    case 58:
    case 8:
      for (size_t i = 0; i < 4; ++i)
        {
          spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &filler_packet,
                                           &packet_response, 1);
          ret.r3_data |= packet_response << (8 * (3 - i));
        }
      ret.class = (cmd_index == 58) ? BEA_SD_R3 : BEA_SD_R7;
      break;
    case 9:
    case 10:
      // Send another byte (no I don't know why)
      spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &filler_packet,
                                       &packet_response, 1);
      for (size_t i = 0; i < 18; ++i)
        {
          spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &filler_packet,
                                           &packet_response, 1);
          ret.r2_data[i] = packet_response;
        }
      // TODO: Actually validate CRC
      ret.class = BEA_SD_R2;
      break;
    default:
      ret.class = BEA_SD_R1;
      break;
    }

  if (spi_err != BEA_SPI_ERROR_NONE)
    {
      ret.err = BEA_SD_ERROR_WRITE;
    }
  else if (spi_err & BEA_SD_R1_CRC_ERR)
    {
      ret.err = BEA_SD_ERROR_CRC;
    }
  else
    {
      ret.err = BEA_SD_ERROR_NONE;
    }
  return ret;
}

__attribute__ ((warn_unused_result)) uint32_t
bea_sd_count_blocks (void)
{
  // Toggle chip select
  bea_gpio_set_value (SD_CHIP_SELECT, true);
  bea_gpio_set_value (SD_CHIP_SELECT, false);
  struct sd_cmd_response resp = sd_send_cmd (9, 0x0);
  uint32_t ret;
  uint64_t hi = 0x0;
  uint64_t lo = 0x0;
  for (size_t i = 0; i < 8; ++i)
    {
      hi <<= 8;
      lo <<= 8;
      hi |= resp.r2_data[i];
      lo |= resp.r2_data[8 + i];
    }
  if (!sd_is_block_addressed)
    {
      uint32_t c_size = (lo >> 62) + ((hi & 0x1F) << 2);
      uint32_t c_size_mult = (lo >> 47) & 0b111;
      ret = (c_size + 1) * c_size_mult;
    }
  else
    {
      uint32_t c_size = (lo >> 48) + ((hi & 0b111111) << 16);
      ret = (c_size + 1) * 1024;
    }
  bea_gpio_set_value (SD_CHIP_SELECT, true);
  return ret;
}

__attribute__ ((warn_unused_result)) enum bea_sd_error
bea_sd_write_blocks (uint32_t n_blocks, uint8_t *buffer, uint32_t start_addr)
{
  bea_gpio_set_value (SD_CHIP_SELECT, false);

  // Need this to enable CRC later
  bea_spi_disable_crc (SD_SPI_CHANNEL);
  struct sd_cmd_response cmd_resp = sd_send_cmd (
      25, (sd_is_block_addressed) ? start_addr : start_addr * 512);
  if (cmd_resp.r1 != 0)
    {
      bea_gpio_set_value (SD_CHIP_SELECT, true);
      return BEA_SD_ERROR_WRITE;
    }
  if (cmd_resp.err != BEA_SD_ERROR_NONE)
    {
      // Just bubble the error upwards
      bea_gpio_set_value (SD_CHIP_SELECT, true);
      return cmd_resp.err;
    }

  // Wait a little bit before we start sending data
  uint8_t dummy_packet = 0xFF;
  uint8_t spi_resp;
  for (size_t i = 0; i < 10; ++i)
    {
      if (bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet, &spi_resp, 1)
          != BEA_SPI_ERROR_NONE)
        {
          bea_gpio_set_value (SD_CHIP_SELECT, true);
          return BEA_SD_ERROR_WRITE;
        }
    }

  uint8_t DATA_TOKEN = 0b11111100;
  uint8_t STOP_TOKEN = 0b11111101;
  enum bea_spi_error spi_err;
  for (size_t i = 0; i < n_blocks; ++i)
    {
      // Send data token
      spi_err
          = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &DATA_TOKEN, &spi_resp, 1);
      // Start CRC calculations
      bea_spi_enable_crc (SD_SPI_CHANNEL, false);
      // Transmit actual data
      spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &buffer[i * 512],
                                       NULL, 512);
      // Transmit 16-bit CRC
      uint16_t crc = bea_spi_get_txcrc (SD_SPI_CHANNEL);
      bea_spi_disable_crc (SD_SPI_CHANNEL);
      uint8_t crc_buf[2] = { crc >> 8, crc & 0xFF };
      uint8_t crc_dummy_buf[2];
      spi_err
          = bea_spi_txrx_blocking (SD_SPI_CHANNEL, crc_buf, crc_dummy_buf, 2);
      // Get the data response
      do
        {
          spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet,
                                           &spi_resp, 1);
        }
      while (spi_resp == 0xFF);
      switch (spi_resp & 0xF)
        {
        case 0b1011:
          // CRC error
          bea_gpio_set_value (SD_CHIP_SELECT, true);
          return BEA_SD_ERROR_CRC;
        case 0b1101:
          // Write error
          bea_gpio_set_value (SD_CHIP_SELECT, true);
          return BEA_SD_ERROR_WRITE;
        case 0b0101:
        // Successful write
        default:
          break;
        }
      // Wait for the card to stop being busy
      do
        {
          spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet,
                                           &spi_resp, 1);
        }
      while (spi_resp == 0x0);
    }

  // Transmit our stop token, wait for the card to finish up
  spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &STOP_TOKEN, &spi_resp, 1);
  do
    {
      spi_err
          = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet, &spi_resp, 1);
    }
  while (spi_resp == 0x0);

  bea_gpio_set_value (SD_CHIP_SELECT, true);
  if (spi_err != BEA_SPI_ERROR_NONE)
    {
      return BEA_SD_ERROR_WRITE;
    }
  return BEA_SD_ERROR_NONE;
}

__attribute__ ((warn_unused_result)) enum bea_sd_error
bea_sd_read_blocks (uint32_t n_blocks, uint8_t *buffer, uint32_t start_addr)
{
  bea_gpio_set_value (SD_CHIP_SELECT, false);

  struct sd_cmd_response resp = sd_send_cmd (
      18, (sd_is_block_addressed) ? start_addr : start_addr * 512);
  if (resp.r1 != 0)
    {
      bea_gpio_set_value (SD_CHIP_SELECT, true);
      return BEA_SD_ERROR_READ;
    }
  uint8_t dummy_packet = 0xFF;
  uint8_t packet_response = 0xFF;
  enum bea_spi_error spi_err;
  for (size_t i = 0; i < n_blocks; ++i)
    {
      do
        {
          spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet,
                                           &packet_response, 1);
        }
      while (packet_response == 0xFF);
      for (size_t j = 0; j < 512; ++j)
        {
          buffer[i * 512 + j] = packet_response;
          spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet,
                                           &packet_response, 1);
        }
      // Ignore CRC
      // TODO: Actually check CRC errors
      spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet,
                                       &packet_response, 1);
      spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet,
                                       &packet_response, 1);
    }
  if (spi_err != BEA_SPI_ERROR_NONE)
    {
      return BEA_SD_ERROR_READ;
    }
  resp = sd_send_cmd (12, 0x0);
  do
    {
      spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet,
                                       &packet_response, 1);
    }
  while (packet_response != 0xFF);
  bea_gpio_set_value (SD_CHIP_SELECT, true);
  return BEA_SD_ERROR_NONE;
}

__attribute__ ((warn_unused_result)) enum bea_sd_error
bea_sd_enable (void)
{
  bea_gpio_set_value (SD_CHIP_SELECT, true);
  uint8_t dummy_packet = 0xFF;
  enum bea_spi_error spi_err;
  for (size_t i = 0; i < 10; ++i)
    {
      spi_err = bea_spi_txrx_blocking (SD_SPI_CHANNEL, &dummy_packet, NULL, 1);
    }
  bea_gpio_set_value (SD_CHIP_SELECT, false);
  struct sd_cmd_response cmd_resp = sd_send_cmd (0x0, 0x0);
  if (cmd_resp.err != BEA_SD_ERROR_NONE)
    {
      bea_gpio_set_value (SD_CHIP_SELECT, true);
      return BEA_SD_ERROR_WRITE;
    }
  else if (cmd_resp.r1 & BEA_SD_R1_CRC_ERR)
    {
      bea_gpio_set_value (SD_CHIP_SELECT, true);
      return BEA_SD_ERROR_CRC;
    }
  else if (cmd_resp.r1 != BEA_SD_R1_IDLE)
    {
      bea_gpio_set_value (SD_CHIP_SELECT, true);
      return BEA_SD_NOT_SD;
    }
  cmd_resp = sd_send_cmd (0x8, 0x0);
  if (cmd_resp.r1 & BEA_SD_R1_ILLEGAL_CMD)
    {
      // Ver 1.x SD or not an SD
      do
        {
          cmd_resp = sd_send_cmd (55, 0x0);
          cmd_resp = sd_send_cmd (41, 0x0);
        }
      while ((cmd_resp.r1 & BEA_SD_R1_IDLE)
             && !(cmd_resp.r1 & BEA_SD_R1_ILLEGAL_CMD));
      if (cmd_resp.r1 & BEA_SD_R1_ILLEGAL_CMD)
        {
          bea_gpio_set_value (SD_CHIP_SELECT, true);
          return BEA_SD_NOT_SD;
        }
      sd_is_block_addressed = false;
    }
  else
    {
      // Ver 2 SD
      do
        {
          cmd_resp = sd_send_cmd (55, 0x0);
          cmd_resp = sd_send_cmd (41, 1 << 30);
        }
      while (cmd_resp.r1 & BEA_SD_R1_IDLE);
      cmd_resp = sd_send_cmd (58, 0x0);
      // Get CCS
      if (cmd_resp.r3_data & (1 << 30))
        {
          // SDXC or SDHC
          sd_is_block_addressed = true;
        }
      else
        {
          // SD standard capacity
          sd_is_block_addressed = false;
        }
    }
  cmd_resp = sd_send_cmd (16, BLOCK_LENGTH_BYTES);
  bea_gpio_set_value (SD_CHIP_SELECT, true);
  if (spi_err == BEA_SPI_ERROR_NONE)
    {
      return BEA_SD_ERROR_NONE;
    }
  else
    {
      return BEA_SD_ERROR_WRITE;
    }
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
    .line = SD_CHIP_DETECT,
  };
  switch (tin.type)
    {
    case BEA_SD_IS_PRESENT:
      bea_gpio_request (&gpio_rq, &resp);
      tout->err = (resp.err == BEA_GPIO_ERROR_NONE) ? BEA_SD_ERROR_NONE
                                                    : BEA_SD_ERROR_GPIO;
      tout->is_present = !resp.value;
      break;
    case BEA_SD_ACTIVATE:
      tout->err = bea_sd_enable ();
      break;
    case BEA_SD_COUNT_BLOCKS:
      tout->block_count = bea_sd_count_blocks ();
      tout->err = BEA_SD_ERROR_NONE;
      break;
    case BEA_SD_READ_BLOCKS:
      tout->err = bea_sd_read_blocks (tin.n_blocks, tin.buffer, tin.block_addr);
      break;
    case BEA_SD_WRITE_BLOCKS:
      tout->err
          = bea_sd_write_blocks (tin.n_blocks, tin.buffer, tin.block_addr);
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
