#ifndef BEATRIX_RCC_H
#define BEATRIX_RCC_H

#include <common.h>
#include <processor.h>

#define BEA_RCC_BASE_ADDR (uint32_t *)(0x58000000)

bool bea_rcc_enable_peripheral (enum bea_io_bank_t io_bank, uint8_t offset);

#endif