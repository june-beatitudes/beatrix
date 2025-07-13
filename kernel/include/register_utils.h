#ifndef BEA_REGISTER_UTILS_H
#define BEA_REGISTER_UTILS_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Set bits in a hardware memory-mapped register (assumed to be 32 bits
 * wide). Writing to an incorrect memory address is undefined behavior and may
 * trigger a hard fault or other interrupt
 *
 * @param base_addr The base memory address of the register
 * @param msb The "highest" bit (inclusive) to set, zero-indexed, big-endian
 * @param lsb The "lowest" bit (inclusive) to set, zero-indexed, big-endian
 * @param value The value to write into that range of the register. Only the
 * lowest `(1 + msb - lsb)` bits will be written, all others will be discarded
 */
void bea_set_reg_bits (volatile uint32_t *base_addr, uint8_t msb, uint8_t lsb,
                       uint32_t value);

/**
 * @brief Get bits in a hardware memory-mapped register (assumed to be 32 bits
 * wide). Reading from an incorrect memory address is undefined behavior and
 * may trigger a hard fault or other interrupt
 *
 * @param base_addr The base memory address of the register
 * @param msb The "highest" bit (inclusive) to read, zero-indexed, big-endian
 * @param lsb The "lowest" bit (inclusive) to read, zero-indexed, big-endian
 * @param out A pointer to a location in memory where the register bits will be
 * copied to. Only the lowest `(1 + msb - lsb)` will be significant, all higher
 * bits will simply be set to zero
 */
uint32_t bea_get_reg_bits (volatile uint32_t *base_addr, uint8_t msb,
                           uint8_t lsb);

#endif
