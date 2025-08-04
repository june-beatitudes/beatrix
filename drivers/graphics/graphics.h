/**
 * @file
 * @author Juniper Beatitudes <juniper@eyes-like-fire.org>
 * @brief Driver for higher-level operations for rendering to the screen
 * @date 2025-07-26
 *
 */

#ifndef BEA_GRAPHICS_H
#define BEA_GRAPHICS_H

#include <driver.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Type of driver request being made from userspace
 *
 */
enum bea_graphics_request_type
{
  /// Draw the current framebuffer to the screen
  BEA_GRAPHICS_UPDATE_DISPLAY,
  /// Draw a bitmap at a specified position on the display
  BEA_GRAPHICS_DRAW_BMP,
  /// Clear the screen with a specific color
  BEA_GRAPHICS_CLEAR_SCREEN,
  /// Render text at a specified position on the display
  BEA_GRAPHICS_DRAW_TEXT,
};

/**
 * @brief Argument for a request being made to the driver from userspace
 *
 */
struct bea_graphics_request_arg
{
  /// The type of request being made
  enum bea_graphics_request_type type;
  /// The x-coordinate of the upper-left corner to draw at (for drawing
  /// operations)
  int16_t x;
  /// The y-coordinate of the upper-left corner to draw at (for drawing
  /// operations)
  int16_t y;
  /// The width of the bitmap to draw to the screen (for bitmap rendering
  /// operations)
  uint8_t width;
  /// The height of the bitmap to draw to the screen (for bitmap rendering
  /// operations)
  uint8_t height;
  /// For bitmap rendering operations, the raw data to draw to the screen
  /// (left-to-right, top-to-bottom); for text rendering operations, the text to
  /// be drawn to the screen
  const uint8_t *buf;
  /// The number of characters to render (for text-rendering operations)
  size_t n_chars;
  /// Color to clear the screen to (white is true, black is false)
  bool clear_color;
  /// Reserved for future use
  bool centered;
};

/**
 * @brief Response from the driver to a request made from userspace
 *
 */
struct bea_graphics_request_response
{
  /// Boolean indicating whether the request succeeded
  bool succeeded;
  /// The total width drawn (for drawing operations)
  uint8_t width_drawn;
  /// The total height drawn (for drawing operations)
  uint8_t height_drawn;
};

#ifdef BEA_KERNEL_INTERNAL

/**
 * @brief A coordinate representing a pixel on a bitmap or on the screen
 * (left-to-right, top-to-bottom)
 *
 */
struct bitmap_coord
{
  int16_t x;
  int16_t y;
};

/**
 * @brief Directly update the display from the internal framebuffer
 */
void bea_update_display ();

/**
 * @brief Render a bitmap directly to the screen
 *
 * @param loc The location of the upper-left corner of the bitmap
 * @param width The width of the bitmap
 * @param height The height of the bitmap
 * @param data The raw bitmap data
 */
void bea_render_bitmap (struct bitmap_coord loc, uint8_t width, uint8_t height,
                        const uint8_t *data);

/**
 * @brief Render left-aligned text directly to the screen
 *
 * @param loc The top-left corner of the text box
 * @param text The text to be rendered
 * @param n The number of characters to be rendered
 */
void bea_render_text (struct bitmap_coord loc, const uint8_t *text, size_t n);

/**
 * @brief Flood the framebuffer with a specific color
 *
 * @param color true for "white", false for "black"
 */
void bea_clear_screen (bool color);

bool bea_graphics_initialize (void);
bool bea_graphics_deinitialize (void);
void bea_graphics_request (void *request, void *response);

extern const struct bea_driver BEA_GRAPHICS_DRIVER;

#endif

#endif
