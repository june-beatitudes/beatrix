#include <driver.h>
#include <graphics/graphics.h>
#include <syscall.h>
#include <user_syscall.h>
#include <version.h>

void
shell_main (__attribute__ ((unused)) void *_)
{
  shell_do_syscall_0arg (BEA_SYSCALL_YIELD);
  const char *GFX_DRIVER_NAME = "graphics";
  uint32_t gfx_driver_idx = shell_do_syscall_1arg (BEA_SYSCALL_FIND_DRIVER,
                                                   (uint32_t)GFX_DRIVER_NAME);
  struct bea_graphics_request_arg rq = {
    .type = BEA_GRAPHICS_DRAW_TEXT,
    .x = 0,
    .y = 0,
    .buf = (const uint8_t *)bea_get_version().name,
    .n_chars = sizeof (bea_get_version().name) - 1,
  };
  struct bea_graphics_request_response resp;
  shell_do_syscall_3arg (BEA_SYSCALL_DRIVER_RQ, gfx_driver_idx, (uint32_t)&rq,
                         (uint32_t)&resp);
  rq.type = BEA_GRAPHICS_UPDATE_DISPLAY;
  shell_do_syscall_3arg (BEA_SYSCALL_DRIVER_RQ, gfx_driver_idx, (uint32_t)&rq,
                         (uint32_t)&resp);
  for (;;)
    {
    }
}