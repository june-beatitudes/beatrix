#include <logging.h>
#include <processor.h>
#include <rcc.h>
#include <register_utils.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <uart.h>

bool
bea_uart_initialize (uint32_t baud)
{
  bea_rcc_enable_peripheral (BEA_APB2, 14);
  bea_set_reg_bits (BEA_RCC_BASE_ADDR, 8, 8, 1);
  while (bea_get_reg_bits (BEA_RCC_BASE_ADDR, 10, 10) != 1)
    ;
  bea_set_reg_bits (BEA_RCC_BASE_ADDR + 0x22, 1, 0, 0b10);
  // Set word length (here, 8 bits), see RM0434, Rev. 1, pp. 1085-1086
  bea_set_reg_bits (BEA_UART_BASE_ADDR + BEA_UART_CR1_OFFSET, 28, 28, 0);
  bea_set_reg_bits (BEA_UART_BASE_ADDR + BEA_UART_CR1_OFFSET, 12, 12, 0);
  // Set baud rate
  bea_set_reg_bits (BEA_UART_BASE_ADDR + BEA_UART_BRR_OFFSET, 15, 0, 1667);
  // Set number of stop bits (here, just 1)
  bea_set_reg_bits (BEA_UART_BASE_ADDR + BEA_UART_CR2_OFFSET, 13, 12, 0);
  // And enable the UART
  bea_set_reg_bits (BEA_UART_BASE_ADDR + BEA_UART_CR1_OFFSET, 0, 0, 1);
  return true;
}

bool
bea_uart_send (const uint8_t *msg)
{
  bea_log (BEA_LOG_INFO, (char *)msg);
  bea_set_reg_bits (BEA_UART_BASE_ADDR + BEA_UART_CR1_OFFSET, 3, 3, 1);
  for (size_t index = 0; msg[index] != '\0'; ++index)
    {
      while (bea_get_reg_bits (BEA_UART_BASE_ADDR + BEA_UART_ISR_OFFSET, 7, 7)
             != 1)
        ;
      bea_set_reg_bits (BEA_UART_BASE_ADDR + BEA_UART_TDR_OFFSET, 7, 0,
                        msg[index]);
    }
  while (bea_get_reg_bits (BEA_UART_BASE_ADDR + BEA_UART_ISR_OFFSET, 6, 6)
         != 1)
    ;
  bea_set_reg_bits (BEA_UART_BASE_ADDR + 0x6, 1, 1, 1);
  return true;
}
