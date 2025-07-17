/**
 * @file
 * @author Juniper Beatitudes
 * @date 2025-07-16
 * @brief Implements a very basic USART transmission interface
 */

#ifndef BEA_UART_H
#define BEA_UART_H

#include <stdbool.h>
#include <stdint.h>

#define BEA_UART_BASE_ADDR (uint32_t *)(0x40013800)
#define BEA_UART_CR1_OFFSET 0x0
#define BEA_UART_BRR_OFFSET 0x3
#define BEA_UART_CR2_OFFSET 0x1
#define BEA_UART_TDR_OFFSET 0xA
#define BEA_UART_ISR_OFFSET 0x7

/**
 * @brief Initializes the UART transmission registers for use by the kernel
 *
 * @param baud The baud rate to be used for transmissions
 * @return A boolean indicating whether or not initialization was successful
 */
bool bea_uart_initialize (uint32_t baud);

/**
 * @brief Sends a null-terminated string of bytes over the UART interface,
 * using the baud rate estabklished in `bea_uart_initialize`
 *
 * @param msg A null-terminated string of bytes to send
 * @returns A boolean indicating whether or not transmission was successful
 */
bool bea_uart_send (const uint8_t *msg);

#endif
