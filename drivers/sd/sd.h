/**
 * @file
 * @brief Presents a driver interface to the Flipper Zero's SD card hardware
 * @author Juniper Beatitudes <juniper@eyes-like-fire.org>
 * @date 2025-07-18
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Type of SD driver userspace request
 *
 */
enum bea_sd_request_type
{
  /// Query if an SD card is inserted
  BEA_SD_IS_PRESENT,
  /// Enable and initialize an inserted SD card
  BEA_SD_ACTIVATE,
  /// Count size of the SD card in 512-byte blocks
  BEA_SD_COUNT_BLOCKS,
  /// Read data (in multiples of 512 bytes) from a specific block address on the
  /// card
  BEA_SD_READ_BLOCKS,
  /// Write data (in multiples of 512 bytes) to a specific block address on the
  /// card
  BEA_SD_WRITE_BLOCKS,
};

/**
 * @brief Errors that can be encountered when processing an SD driver request
 *
 */
enum bea_sd_error
{
  /// No error encountered
  BEA_SD_ERROR_NONE,
  /// Tried to activate card, but it was not an SD card
  BEA_SD_NOT_SD,
  /// GPIO operation failed
  BEA_SD_ERROR_GPIO,
  /// Read operation failed
  BEA_SD_ERROR_READ,
  /// Write operation failed
  BEA_SD_ERROR_WRITE,
  /// CRC mismatch encountered while processing a request
  BEA_SD_ERROR_CRC,
};

/**
 * @brief An argument passed to the driver's `request` function from userspace
 *
 */
struct bea_sd_request_arg
{
  /// Type of request
  enum bea_sd_request_type type;
  /// Number of 512-byte blocks to be read or written
  size_t n_blocks;
  /// Block address to read or write at
  size_t block_addr;
  /// Buffer to read or write from
  uint8_t *buffer;
};

/**
 * @brief A response passed back to userspace from the driver's `request`
 * function
 *
 */
struct bea_sd_request_response
{
  /// An error encountered while processing the request (if any)
  enum bea_sd_error err;
  /// Boolean indicating whether or not an SD card is present (if that was being
  /// queried)
  bool is_present;
  /// Number of blocks present on the card (if that was being queried)
  uint32_t block_count;
};

#ifdef BEA_KERNEL_INTERNAL

/**
 * @brief Function that counts the number of 512-byte blocks available on the
 * (inserted, activated) SD card
 *
 * @return The number of blocks
 * @internal
 *
 */
__attribute__ ((warn_unused_result)) uint32_t bea_sd_count_blocks (void);

/**
 * @brief Read `n_blocks` blocks at `start_addr` into `buffer` from the
 * (inserted, activated) SD card
 *
 * @return Any error encountered
 * @param n_blocks The number of blocks to read
 * @param buffer THe buffer to read the data into (size must be equal to 512 *
 * `n_blocks`)
 * @param start_addr The block address to start the read operation at
 * @internal
 *
 */
__attribute__ ((warn_unused_result)) enum bea_sd_error
bea_sd_read_blocks (uint32_t n_blocks, uint8_t *buffer, uint32_t start_addr);

/**
 * @brief Write `n_blocks`  from `buffer` onto the (inserted, activated) SD card
 * at `start_addr`
 *
 * @return Any error encountered
 * @param n_blocks The number of blocks to write
 * @param buffer The buffer to write from (size must be equal to 512 *
 * `n_blocks`)
 * @param start_addr The block address to start the write operation as
 */
__attribute__ ((warn_unused_result)) enum bea_sd_error
bea_sd_write_blocks (uint32_t n_blocks, uint8_t *buffer, uint32_t start_addr);

/**
 * @brief Activate an (inserted) SD card, going through the appropriate
 * initialization procedute
 *
 * @return Any errors encountered
 *
 */
__attribute__ ((warn_unused_result)) enum bea_sd_error bea_sd_enable (void);

bool bea_sd_initialize (void);
bool bea_sd_deinitialize (void);
void bea_sd_request (void *arg, void *out);

/// Entry in the kernel driver table
extern const struct bea_driver BEA_SD_DRIVER;

#endif
