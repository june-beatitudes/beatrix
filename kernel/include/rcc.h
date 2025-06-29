#ifndef BEATRIX_RCC_H
#define BEATRIX_RCC_H

#include <common.h>
#include <processor.h>

#define BEA_RCC_BASE_ADDR (uint32_t *)(0x58000000)

/**
 * @brief Enable a specific peripheral in the RCC subsystem
 *
 * @param io_bank The I/O bank the peripheral is in (e.g., AHB1, AHB2, etc.)
 * @param offset The offset of the peripheral in the bank's register. APB1 has
 * two registers; to access the second register, pass in the offset in that
 * register plus 32
 * @return true Peripheral enable successful (within reason to check)
 * @return false Peripheral enable unsuccessful
 */
bool bea_rcc_enable_peripheral (enum bea_io_bank_t io_bank, uint8_t offset);

#endif