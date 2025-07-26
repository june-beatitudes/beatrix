#include <driver.h>
#include <gpio/gpio.h>
#include <rcc.h>
#include <register_utils.h>

enum gpio_reg_offset
{
  GPIO_REG_MODER = 0x0,
  GPIO_REG_OTYPER = 0x1,
  GPIO_REG_OSPEEDR = 0x2,
  GPIO_REG_PUPDR = 0x3,
  GPIO_REG_IDR = 0x4,
  GPIO_REG_ODR = 0x5,
  GPIO_REG_BSRR = 0x6,
  GPIO_REG_LCKR = 0x7,
  GPIO_REG_AFRL = 0x8,
  GPIO_REG_AFRH = 0x9,
  GPIO_REG_BRR = 0xA,
};

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

__attribute__ ((always_inline)) static inline bool
gpio_line_exists (struct bea_gpio_line line)
{
  if (line.pin > 15)
    {
      return false;
    }
  else if (line.bank == BEA_GPIO_BANK_E && line.pin > 4)
    {
      return false;
    }
  else if (line.bank == BEA_GPIO_BANK_H && (line.pin == 2 || line.pin > 3))
    {
      return false;
    }
  return true;
}

enum bea_gpio_error
bea_gpio_set_mode (struct bea_gpio_line line, enum bea_gpio_pin_mode mode)
{
  if (!gpio_line_exists (line))
    {
      return BEA_GPIO_INVALID_LINE;
    }
  bea_set_reg_bits ((uint32_t *)line.bank + GPIO_REG_MODER, line.pin * 2 + 1,
                    line.pin * 2, mode);
  return BEA_GPIO_ERROR_NONE;
}

enum bea_gpio_error
bea_gpio_set_alt_func (struct bea_gpio_line line, enum bea_gpio_alt_func alt_func)
{
  if (!gpio_line_exists (line))
    {
      return BEA_GPIO_INVALID_LINE;
    }
  if (line.pin < 8)
    {
      bea_set_reg_bits ((uint32_t *)line.bank + GPIO_REG_AFRL, line.pin * 4 + 3,
                        line.pin * 4, alt_func);
    }
  else
    {
      bea_set_reg_bits ((uint32_t *)line.bank + GPIO_REG_AFRH,
                        (line.pin - 8) * 4 + 3, (line.pin - 8) * 4, alt_func);
    }
  return BEA_GPIO_ERROR_NONE;
}

enum bea_gpio_error
bea_gpio_set_value (struct bea_gpio_line line, bool value)
{
  if (!gpio_line_exists (line))
    {
      return BEA_GPIO_INVALID_LINE;
    }
  bea_set_reg_bits ((uint32_t *)line.bank + GPIO_REG_ODR, line.pin, line.pin,
                    value);
  return BEA_GPIO_ERROR_NONE;
}

enum bea_gpio_error
bea_gpio_read_value (struct bea_gpio_line line, bool *out)
{
  if (!gpio_line_exists (line))
    {
      return BEA_GPIO_INVALID_LINE;
    }
  *out = (bool)bea_get_reg_bits ((uint32_t *)line.bank + GPIO_REG_IDR, line.pin,
                                 line.pin);
  return BEA_GPIO_ERROR_NONE;
}

enum bea_gpio_error
bea_gpio_set_pulldir (struct bea_gpio_line line,
                      enum bea_gpio_pull_direction dir)
{
  if (!gpio_line_exists (line))
    {
      return BEA_GPIO_INVALID_LINE;
    }
  bea_set_reg_bits ((uint32_t *)line.bank + GPIO_REG_PUPDR, line.pin * 2 + 1,
                    line.pin * 2, dir);
  return BEA_GPIO_ERROR_NONE;
}

void
bea_gpio_request (void *request, void *retval)
{
  struct bea_gpio_request_arg arg = *((struct bea_gpio_request_arg *)request);
  struct bea_gpio_request_response resp;

  switch (arg.type)
    {
    case BEA_GPIO_SET_MODE:
      resp.err = bea_gpio_set_mode (arg.line, arg.mode);
      break;
    case BEA_GPIO_SET_ALT_FUNC:
      resp.err = bea_gpio_set_alt_func (arg.line, arg.alt_func);
      break;
    case BEA_GPIO_SET_VALUE:
      resp.err = bea_gpio_set_value (arg.line, arg.value);
      break;
    case BEA_GPIO_READ_VALUE:
      resp.err = bea_gpio_read_value (arg.line, &resp.value);
      break;
    case BEA_GPIO_SET_PULLDIR:
      resp.err = bea_gpio_set_pulldir (arg.line, arg.pulldir);
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
