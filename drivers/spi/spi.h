/**
 * @file
 * @brief Presents a driver interface to the SPI buses on the STM32WB55xx
 * processor
 * @author Juniper Beatitudes <juniper@eyes-like-fire.org>
 * @date 2025-07-16
 */

#ifndef BEA_SPI_H
#define BEA_SPI_H

#include <driver.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Various types of driver requests that can be made from userspace
 *
 */
enum bea_spi_request_type
{
  /// Transmit and receive a number of bytes
  BEA_SPI_TXRX,
  /// Enable hardware CRC generation
  BEA_SPI_ENABLE_CRC,
  /// Disable hardware CRC generation
  BEA_SPI_DISABLE_CRC,
  /// Fetch hardware CRC value for transmitted bytes
  BEA_SPI_GET_TXCRC,
  /// Fetch hardware CRC value for received bytes
  BEA_SPI_GET_RXCRC,
};

/**
 * @brief SPI channels available in hardware
 *
 */
enum bea_spi_channel : uint32_t
{
  /// SPI channel 1 (channel R in the official F0 schematics)
  BEA_SPI_CHAN1 = 0x40013000,
  /// SPI channel 2 (channel D in the official F0 schematics)
  BEA_SPI_CHAN2 = 0x40003800,
};

/**
 * @brief Argument to a driver request made from userspace
 *
 */
struct bea_spi_request_arg
{
  /// The type of request being made
  enum bea_spi_request_type type;
  /// The relevant SPI channel
  enum bea_spi_channel channel;
  /// RX output buffer (for TXRX operations). Passing in NULL means any read
  /// values will be ignored
  uint8_t *rx_buf;
  /// TX input buffer (for TXRX operations)
  uint8_t *tx_buf;
  /// The number of bytes to be read and written (for TXRX operations)
  size_t n_bytes;
  /// Boolean indicating if CRC should be enabled in 16-bit or 8-bit mode (for
  /// CRC enable operations)
  bool is_crc8;
};

/**
 * @brief Various types of errors that can be encountered during SPI operations
 *
 * @todo Add proper timeout checks
 */
enum bea_spi_error
{
  /// No error encountered
  BEA_SPI_ERROR_NONE,
  /// Operation timed out
  BEA_SPI_TIMEOUT,
  /// Reserved for future use
  BEA_SPI_WOULDBLOCK,
};

/**
 * @brief Response to a request to the driver from userspace
 *
 */
struct bea_spi_request_response
{
  /// Any error encountered serving the request
  enum bea_spi_error err;
  /// The CRC value read (if relevant)
  uint16_t crc;
};

#ifdef BEA_KERNEL_INTERNAL

/**
 * @brief Make a blocking request to read and write bytes from an SPI channel
 *
 * @return Any error encountered
 * @param channel The channel to read/write from
 * @param tx The buffer to transmit bytes from
 * @param rx The buffer to read bytes into. If NULL is passed in, any bytes read
 * are simply ignored
 * @param n The number of bytes to read from the channel
 */
__attribute__ ((warn_unused_result)) enum bea_spi_error
bea_spi_txrx_blocking (enum bea_spi_channel channel, uint8_t *tx, uint8_t *rx,
                       size_t n);

/**
 * @brief Enable hardware CRC calculations
 *
 * @param channel The channel to enable calculations for
 * @param is_crc8 If true, does CRC8 generation; otherwise, does CRC16
 */
void bea_spi_enable_crc (enum bea_spi_channel channel, bool is_crc8);

/**
 * @brief Disable hardware CRC calculation
 *
 * @param channel The channel to disable CRC calculation for
 */
void bea_spi_disable_crc (enum bea_spi_channel channel);

/**
 * @brief Retrieve the hardware-calculated TX CRC value
 *
 * @return The read value from hardware; if using CRC8, ignore the upper 8 bits
 * @param channel The channel to read from
 */
__attribute__ ((warn_unused_result)) uint16_t
bea_spi_get_txcrc (enum bea_spi_channel channel);

/**
 * @brief Retrieve the hardware-calculated RX CRC value
 *
 * @return The read value from hardware; if using CRC8, ignore the upper 8 bits
 * @param channel The channel to read from
 */
__attribute__ ((warn_unused_result)) uint16_t
bea_spi_get_rxcrc (enum bea_spi_channel channel);

bool bea_spi_initialize (void);
bool bea_spi_deinitialize (void);
void bea_spi_request (void *arg, void *result);

/// Entry in the kernel driver table
const extern struct bea_driver BEA_SPI_DRIVER;

#endif

#endif
