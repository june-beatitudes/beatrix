#include <bootloader.h>
#include <driver_table.h>
#include <ff.h>
#include <graphics/graphics.h>
#include <logging.h>
#include <processor.h>
#include <register_utils.h>
#include <sd/sd.h>
#include <shell_main.h>
#include <stddef.h>
#include <stdint.h>
#include <task.h>

extern const uint8_t bea_logo[512];

void
bea_do_boot (void)
{
  for (size_t i = 0; BEA_DRIVER_TABLE[i]->name[0] != '\0'; ++i)
    {
      bea_log (BEA_LOG_INFO, BEA_DRIVER_TABLE[i]->name);
      if (!BEA_DRIVER_TABLE[i]->initialize ())
        {
          bea_log (BEA_LOG_ERROR, "Driver initialization failed!");
        }
      else
        {
          bea_log (BEA_LOG_INFO, "Driver initialization successful");
        }
    }
  struct bea_graphics_request_arg drawrq = {
    .type = BEA_GRAPHICS_DRAW_BMP,
    .x = 31,
    .y = 0,
    .buf = bea_logo,
    .width = 64,
    .height = 64,
  };
  struct bea_graphics_request_response _;
  BEA_DRIVER_TABLE[4]->request (&drawrq, &_);
  struct bea_graphics_request_arg updaterq = {
    .type = BEA_GRAPHICS_UPDATE_DISPLAY,
  };
  BEA_DRIVER_TABLE[4]->request (&updaterq, &_);
  // bea_init_scheduler ();
  // uint32_t *dummy_stack = (uint32_t *)0x20030000;
  // uint32_t *stack1 = dummy_stack + 256;
  // uint32_t *stack2 = stack1 + 1024;
  // uint32_t *stack3 = stack2 + 1024;
  // uint32_t *heap1 = stack3 + 1024;
  // uint32_t *heap2 = heap1 + 256;
  // uint32_t *heap3 = heap2 + 256;
  // bea_spawn_task (1, NULL, NULL, dummy_stack, 256, NULL, 0,
  //                 BEA_PARENTLESS_INDEX);
  // bea_spawn_task (1024, shell_main2, NULL, stack2, 1024, heap2, 256,
  //                 BEA_PARENTLESS_INDEX);
  // bea_spawn_task (4096, shell_main, NULL, stack1, 1024, heap1, 256,
  //                 BEA_PARENTLESS_INDEX);
  // __asm__ volatile ("MOV R0, %[stack_top]\n"
  //                   "MSR PSP, R0\n"
  //                   "LDR R0, =0b111\n"
  //                   "MSR CONTROL, R0\n"
  //                   "ISB\n"
  //                   "SVC #5\n"
  //                   :
  //                   : [stack_top] "r"(dummy_stack + 256));
  // bea_do_scheduler();
  __asm__ volatile ("_bdb_loop0:\n"
                    "B _bdb_loop0\n");
}
