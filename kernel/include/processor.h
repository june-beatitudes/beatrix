#ifndef BEATRIX_PROCESSOR_H
#define BEATRIX_PROCESSOR_H

#include <stdint.h>

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

/**
 * @brief Sends a semihost request to whatever device is connected (if
 * semihosting is enabled)
 *
 * @param reason The semihost request "reason", i.e. the request type
 * @param arg The semihost request argument
 *
 * @return The semihost request result from the processor/device
 */
int32_t bea_semihost_rq (int32_t reason, void *arg);

/**
 * @brief Represents a semihost request type
 */
enum bea_semihost_reason_t
{
  /// Write a null-terminated string directly to the semihosting terminal
  /// output
  BEA_SEMIHOST_SYSWRITEZ = 0x04,
};

#endif
