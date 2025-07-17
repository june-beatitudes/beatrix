#include <driver.h>
#include <gpio/gpio.h>
#include <rcc.h>
#include <register_utils.h>

bool
bea_gpio_initialize ()
{
  // Enable peripheral clocks for GPIO banks A-E...
  for (size_t i = 0; i < 5; ++i)
    {
      bea_rcc_enable_peripheral (BEA_AHB2, i);
    }
  // and H
  bea_rcc_enable_peripheral (BEA_AHB2, 7);
  return true;
}

bool
bea_gpio_deinitialize ()
{
  // Do nothing
  return true;
}

void
bea_gpio_request (void *request, void *retval)
{
  struct bea_gpio_request_arg arg = *((struct bea_gpio_request_arg *)request);
  struct bea_gpio_request_response resp;

  switch (arg.type)
    {
    case BEA_GPIO_SET_MODE:
      bea_set_reg_bits ((uint32_t *)arg.bank, arg.pin * 2 + 1, arg.pin * 2,
                        (uint32_t)arg.mode);
      if ((arg.mode & 0b11) == 0b00)
        {
          bea_set_reg_bits ((uint32_t *)(arg.bank + 0x0C), arg.pin * 2 + 1,
                            arg.pin * 2, ((uint32_t)arg.mode >> 2));
        }
      resp.succeeded = true;
      break;
    case BEA_GPIO_SET_VALUE:
      bea_set_reg_bits ((uint32_t *)(arg.bank + 0x14), arg.pin, arg.pin,
                        (uint32_t)arg.value);
      resp.succeeded = true;
      break;
    case BEA_GPIO_READ_VALUE:
      resp.value = (bool)bea_get_reg_bits ((uint32_t *)(arg.bank + 0x10),
                                           arg.pin, arg.pin);
      resp.succeeded = true;
      break;
    }

  *((struct bea_gpio_request_response *)retval) = resp;
}

const struct bea_driver BEA_GPIO_DRIVER = {
  .name = "gpio",
  .initialize = bea_gpio_initialize,
  .deinitialize = bea_gpio_deinitialize,
  .request = bea_gpio_request,
};
