/**
 * @file
 * @brief Presents a driver interface to the SPI bus on the STM32WB55xx
 * processor
 * @author Juniper Beatitudes
 * @date 2025-07-16
 */

#ifndef BEA_SPI_H
#define BEA_SPI_H

#include <driver.h>
#include <stdbool.h>
#include <stdint.h>

bool bea_spi_initialize (void);
bool bea_spi_deinitialize (void);

void bea_spi_request (void *arg, void *result);

enum bea_spi_request_type
{
  BEA_SPI_INIT_CHANNEL,
  BEA_SPI_TXRX,
  BEA_SPI_DEINIT_CHANNEL,
};

enum bea_spi_channel : uint32_t
{
  BEA_SPI_CHAN1 = 0x40013000,
  BEA_SPI_CHAN2 = 0x40003800,
};

enum bea_spi_comm_mode
{
  BEA_SPI_FULL_DUPLEX,
  BEA_SPI_HALF_DUPLEX_OUT_ENABLED,
  BEA_SPI_HALF_DUPLEX_OUT_DISABLED,
  BEA_SPI_SIMPLEX,
};

struct bea_spi_channel_config
{
  enum bea_spi_channel chan;
  enum bea_spi_comm_mode comm_mode;
};

struct bea_spi_request_arg
{
  enum bea_spi_request_type type;
  struct bea_spi_channel_config channel_cfg;
  uint8_t packet;
  bool ignore_resp;
};

struct bea_spi_request_response
{
  bool succeeded;
  uint8_t packet;
};

const extern struct bea_driver BEA_SPI_DRIVER;

#endif
