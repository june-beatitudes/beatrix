#include <common.h>
#include <processor.h>
#include <rcc.h>

bool
bea_rcc_enable_peripheral (enum bea_io_bank_t io_bank, uint8_t offset)
{
  if ((io_bank == BEA_APB1 && offset > 63) || offset > 31)
    {
      // Get better arguments
      return false;
    }
  uint32_t reg_offset = 0;
  switch (io_bank)
    {
    case BEA_APB1:
      if (offset > 31)
        {
          // There are actually two registers, which I dislike b/c they don't
          // need to be that way
          reg_offset = 0x05C;
          offset -= 32; // Makes the rest of this code simpler
        }
      else
        {
          reg_offset = 0x058;
        }
      break;
    case BEA_APB2:
      reg_offset = 0x060;
      break;
    case BEA_APB3:
      // Only for CPU2
      reg_offset = 0x164;
      break;
    case BEA_AHB1:
      reg_offset = 0x048;
      break;
    case BEA_AHB2:
      reg_offset = 0x04C;
      break;
    case BEA_AHB3:
      reg_offset = 0x050;
      break;
    }
  *(BEA_RCC_BASE_ADDR + reg_offset) |= (1 << offset);
  return true;
}