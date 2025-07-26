#include <register_utils.h>
#include <stdbool.h>
#include <stdint.h>

__attribute__ ((always_inline)) inline void
bea_set_reg_bits (volatile uint32_t *base_addr, uint8_t msb, uint8_t lsb,
                  uint32_t value)
{
  if (msb > 31 || msb < lsb)
    {
      return;
    }
  else if (msb == 31 && lsb == 0)
    {
      *base_addr = value;
      return;
    }
  uint32_t mask = ((1 << (msb - lsb + 1)) - 1) << lsb;
  *base_addr = (*base_addr & ~mask) | ((value << lsb) & mask);
}

__attribute__ ((always_inline)) inline uint32_t
bea_get_reg_bits (volatile uint32_t *base_addr, uint8_t msb, uint8_t lsb)
{
  if (msb > 31 || msb < lsb)
    {
      // You get what you pay for, which is nothing
      return 0U;
    }
  else if (msb == 31 && lsb == 0)
    {
      return *base_addr;
    }
  uint32_t mask = ((1 << (msb - lsb + 1)) - 1) << lsb;
  return (*base_addr & mask) >> lsb;
}
