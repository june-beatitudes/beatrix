/**
 * @file
 * @brief Low-level driver for direct display access
 * @author Juniper Beatitudes <juniper@eyes-like-fire.org>
 * @date 2025-07-17
 */

#ifndef BEA_DISPLAY_DIRECT_H
#define BEA_DISPLAY_DIRECT_H

#include <gpio/gpio.h>
#include <spi/spi.h>

/**
 * @brief Type of request being made to the driver from userspace
 *
 */
enum bea_display_direct_request_type
{
  /// Render a framebuffer directly to the screen
  BEA_DISPLAY_DIRECT_RENDER,
};

/**
 * @brief Argument to a userspace request to the driver
 *
 */
struct bea_display_direct_request_arg
{
  /// Type of request being made
  enum bea_display_direct_request_type type;
  /// Framebuffer (for rendering operations)
  const uint8_t *framebuffer;
};

#ifdef BEA_KERNEL_INTERNAL

/**
 * @brief Render a framebuffer directly to the screen
 *
 * @param framebuffer A 1024 byte array of data to render to the screen
 */
void bea_display_direct_render (const uint8_t *framebuffer);

bool bea_display_direct_initialize (void);
bool bea_display_direct_deinitialize (void);
void bea_display_direct_request (void *request, void *result);

/// Entry in the kernel driver table
extern const struct bea_driver BEA_DISPLAY_DIRECT_DRIVER;

#endif

#endif
