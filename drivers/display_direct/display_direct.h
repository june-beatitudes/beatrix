/**
 * @file
 * @brief Low-level driver for direct display access
 * @author Juniper Beatitudes
 * @date 2025-07-17
 */

#ifndef BEA_DISPLAY_DIRECT_H
#define BEA_DISPLAY_DIRECT_H

#include <gpio/gpio.h>
#include <spi/spi.h>

bool bea_display_direct_initialize (void);
bool bea_display_direct_deinitialize (void);

enum bea_display_direct_request_type
{
  BEA_DISPLAY_DIRECT_RENDER,
};

struct bea_display_direct_request_arg
{
  enum bea_display_direct_request_type type;
  const uint8_t *framebuffer;
};

void bea_display_direct_request (void *request, void *result);

extern const struct bea_driver BEA_DISPLAY_DIRECT_DRIVER;

#endif
