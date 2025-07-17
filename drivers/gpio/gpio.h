/**
 * @file
 * @brief Presents a driver interface to the GPIO functionality available on
 * the Flipper
 * @author Juniper Beatitudes
 * @date 2025-07-16
 */

#ifndef BEA_GPIO_H
#define BEA_GPIO_H

#include <processor.h>
#include <rcc.h>
#include <stdbool.h>

/**
 * @brief Initializes the GPIO clock banks
 * @return A boolean indicating whether initialization was successful
 */
bool bea_gpio_initialize (void);

/**
 * @brief Deinitializes the driver
 * @return A boolean indicating whether deinitialization was successful
 */
bool bea_gpio_deinitialize (void);

/**
 * @brief Makes a request (from userspace) to the GPIO driver
 *
 * @param arg The argument to the request (here a void pointer, actually a
 * `bea_gpio_request_arg` pointer)
 * @param result Out parameter for the result of the request (here a void
 * pointer, actually a `bea_gpio_request_response` pointer)
 */
void bea_gpio_request (void *arg, void *result);

/// Entry in the kernel driver table
extern const struct bea_driver BEA_GPIO_DRIVER;

enum bea_gpio_rq_type
{
  BEA_GPIO_SET_MODE,
  BEA_GPIO_SET_VALUE,
  BEA_GPIO_READ_VALUE,
};

enum bea_gpio_bank
{
  BEA_GPIO_BANK_A = 0x48000000,
  BEA_GPIO_BANK_B = 0x48000400,
  BEA_GPIO_BANK_C = 0x48000800,
  BEA_GPIO_BANK_D = 0x48000C00,
  BEA_GPIO_BANK_E = 0x48001000,
  BEA_GPIO_BANK_H = 0x48001C00,
};

enum bea_gpio_pin_mode
{
  BEA_GPIO_OUT = 0b0001,
  BEA_GPIO_ALT = 0b0010,
  BEA_GPIO_PULLUP = 0b0100,
  BEA_GPIO_PULLDOWN = 0b1000,
  BEA_GPIO_ANALOG = 0b0000,
  BEA_GPIO_FLOATING = 0b0011,
};

struct bea_gpio_request_arg
{
  enum bea_gpio_rq_type type;
  enum bea_gpio_bank bank;
  uint8_t pin;
  union
  {
    bool value;
    enum bea_gpio_pin_mode mode;
  };
};

struct bea_gpio_request_response
{
  bool succeeded;
  bool value;
};

#endif
