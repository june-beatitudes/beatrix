#ifndef BEATRIX_ISR_H
#define BEATRIX_ISR_H

#define BEA_KERNEL_STACK_TOP 0x20030000

/**
 * @brief Represents an ISR
 */
typedef void (*isr_t) (void);

/// Contains all of the ISRs for the entire system; loaded into the bottom of
/// flash memory
extern isr_t const ISR_TABLE[];

#endif
