/**
 * @file
 * @brief Presents a driver interface to the Flipper Zero's SD card hardware
 * @author Juniper Beatitudes
 * @date 2025-07-18
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool bea_sd_initialize (void);
bool bea_sd_deinitialize (void);

enum bea_sd_request_type
{
  BEA_SD_IS_PRESENT,
  BEA_SD_ACTIVATE,
  BEA_SD_GET_INFO,
  BEA_SD_READ_BLOCKS,
  BEA_SD_WRITE_BLOCKS,
};

struct bea_sd_request_arg
{
  enum bea_sd_request_type type;
  // Used only for reads and writes
  size_t n_blocks;
  size_t block_addr;
  uint8_t *buffer;
};

struct bea_sd_info
{
};

struct bea_sd_request_response
{
  bool succeeded;
  bool is_present;
  struct bea_sd_info info;
};

void bea_sd_request (void *arg, void *out);

/// Entry in the kernel driver table
extern const struct bea_driver BEA_SD_DRIVER;
