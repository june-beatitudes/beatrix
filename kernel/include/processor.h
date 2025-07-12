#ifndef BEATRIX_PROCESSOR_H
#define BEATRIX_PROCESSOR_H

/**
 * @brief Represents an I/O peripheral bank
 */
enum bea_io_bank_t
{
  BEA_APB1,
  BEA_APB2,
  BEA_APB3,
  BEA_AHB1,
  BEA_AHB2,
  BEA_AHB3,
};

/**
 * @brief Enables the STM32WB55RGV hardware FPU. Must be called before *any*
 * floating point instructions are executed. Reference STMicroelectronics
 * PM0214 Rev. 10, pp. 257
 */
void bea_fpu_enable ();

#endif
