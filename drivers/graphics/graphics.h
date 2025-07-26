#ifndef BEA_GRAPHICS_H
#define BEA_GRAPHICS_H

#include <driver.h>
#include <stdbool.h>
#include <stdint.h>

bool bea_graphics_initialize (void);
bool bea_graphics_deinitialize (void);

enum bea_graphics_request_type
{
  BEA_GRAPHICS_UPDATE_DISPLAY,
  BEA_GRAPHICS_DRAW_BMP,
  BEA_GRAPHICS_DRAW_TEXT,
};

struct bea_graphics_request_arg
{
  enum bea_graphics_request_type type;
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
  const uint8_t *buf;
  size_t n_chars;
  bool centered;
};

struct bea_graphics_request_response
{
  bool succeeded;
  uint8_t width_drawn;
  uint8_t height_drawn;
};

void bea_graphics_request (void *request, void *response);

extern const struct bea_driver BEA_GRAPHICS_DRIVER;

#endif
