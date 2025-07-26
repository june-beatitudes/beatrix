#include <display_direct/display_direct.h>
#include <graphics/graphics.h>
#include <stdbool.h>
#include <stdint.h>

static uint8_t framebuffer[512];

extern const uint8_t bea_unifont[223][16];

bool
bea_graphics_initialize (void)
{
  for (size_t i = 0; i < 1024; ++i)
    {
      ((uint8_t *)framebuffer)[i] = 0xFF;
    }
  return true;
}
bool
bea_graphics_deinitialize (void)
{
  return true;
}

struct display_coord
{
  uint8_t col;
  uint8_t page;
  uint8_t offset;
};

__attribute__ ((always_inline)) static inline uint8_t
get_pixel (struct bitmap_coord coord, uint8_t width, const uint8_t *bmp)
{
  size_t base_ind = (coord.x + coord.y * width) / 8;
  size_t offset = 7 - ((coord.x + coord.y * width) % 8);
  return ((bmp[base_ind] & (1 << offset)) == 0) ? 0 : 1;
}

__attribute__ ((always_inline)) static inline struct display_coord
bmp2display (struct bitmap_coord coord)
{
  struct display_coord ret;
  ret.col = coord.x;
  ret.page = (63 - coord.y) / 8;
  ret.offset = (63 - coord.y) % 8;
  return ret;
}

void
bea_render_bitmap (struct bitmap_coord loc, uint8_t width, uint8_t height,
                   const uint8_t *data)
{
  if (loc.x > 127 || loc.y > 63)
    {
      return;
    }
  uint8_t ymax = (loc.y + height > 63) ? 63 : loc.y + height;
  uint8_t xmax = (loc.x + width > 127) ? 127 : loc.x + width;
  for (uint16_t col = loc.x; col <= xmax; ++col)
    {
      for (int8_t row = loc.y; row <= ymax; ++row)
        {
          struct bitmap_coord pixcoord = {
            .x = col,
            .y = row,
          };
          struct bitmap_coord bmpcoord = {
            .x = col - loc.x,
            .y = row - loc.y,
          };
          struct display_coord screencoord = bmp2display (pixcoord);
          framebuffer[screencoord.col * 8 + screencoord.page]
              &= ~(1 << screencoord.offset);
          framebuffer[screencoord.col * 8 + screencoord.page]
              |= get_pixel (bmpcoord, width, data) << screencoord.offset;
        }
    }
}

void
bea_render_text (uint8_t x, uint8_t y, const uint8_t *text, size_t n)
{
  uint8_t current_x = x;
  for (size_t i = 0; i < n; ++i)
    {
      if (text[i] == '\n')
        {
          y += 16;
          current_x = x;
        }
      else if (text[i] > 127 || (text[i] >= 32 && text[i] <= 126))
        {
          uint8_t ind = (text[i] > 127) ? text[i] - 33 : text[i] - 32;
          struct bitmap_coord loc = {
            .x = current_x,
            .y = y,
          };
          bea_render_bitmap (loc, 8, 16, bea_unifont[ind]);
          current_x += 8;
        }
    }
}

void
bea_graphics_request (void *request, void *response)
{
  struct bea_graphics_request_arg arg
      = *((struct bea_graphics_request_arg *)request);
  struct bitmap_coord loc = {
    .x = arg.x,
    .y = arg.y,
  };
  switch (arg.type)
    {
    case BEA_GRAPHICS_UPDATE_DISPLAY:
      bea_display_direct_render ((const uint8_t *)framebuffer);
      break;
    case BEA_GRAPHICS_DRAW_BMP:
      bea_render_bitmap (loc, arg.width, arg.height, arg.buf);
      break;
    case BEA_GRAPHICS_DRAW_TEXT:
      bea_render_text (arg.x, arg.y, arg.buf, arg.n_chars);
      break;
    }
  struct bea_graphics_request_response resp = {
    .succeeded = true,
  };
  *((struct bea_graphics_request_response *)response) = resp;
}

const struct bea_driver BEA_GRAPHICS_DRIVER = {
  .name = "graphics",
  .initialize = bea_graphics_initialize,
  .deinitialize = bea_graphics_deinitialize,
  .request = bea_graphics_request,
};
