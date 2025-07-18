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

/// Type of userspace request
enum bea_gpio_rq_type
{
  /// Set the mode for a particular pin
  BEA_GPIO_SET_MODE,
  /// Set the chosen alternate function for an alternate function mode pin
  BEA_GPIO_SET_ALT_FUNC,
  /// Set the value on an output pin
  BEA_GPIO_SET_VALUE,
  /// Read the value of an input pin
  BEA_GPIO_READ_VALUE,
};

/// GPIO banks on the Flipper, each numbered based on its registers' locations
/// in memory
enum bea_gpio_bank
{
  BEA_GPIO_BANK_A = 0x48000000,
  BEA_GPIO_BANK_B = 0x48000400,
  BEA_GPIO_BANK_C = 0x48000800,
  BEA_GPIO_BANK_D = 0x48000C00,
  BEA_GPIO_BANK_E = 0x48001000,
  BEA_GPIO_BANK_H = 0x48001C00,
};

struct bea_gpio_line
{
  enum bea_gpio_bank bank;
  uint8_t pin;
};

/// Types of GPIO pin modes, with the numbers associated encoding the register
/// contents that they are associated with
enum bea_gpio_pin_mode
{
  BEA_GPIO_OUT = 0b0001,
  BEA_GPIO_ALT = 0b0010,
  BEA_GPIO_PULLUP = 0b0100,
  BEA_GPIO_PULLDOWN = 0b1000,
  BEA_GPIO_ANALOG = 0b0000,
  BEA_GPIO_FLOATING = 0b0011,
};

/**
 * @brief Request sent to the GPIO driver
 *
 * @member type The type of request
 * @member bank The bank being changed/read/updated
 * @member pin The specific pin being changed/read/updated
 * @member value The new value for a pin (for a `SET_VALUE` request)
 * @member mode The new mode for a pin (for a `SET_MODE` request)
 */
struct bea_gpio_request_arg
{
  enum bea_gpio_rq_type type;
  struct bea_gpio_line line;
  union
  {
    bool value;
    enum bea_gpio_pin_mode mode;
    uint8_t alt_func;
  };
};

/**
 * @brief Response from the GPIO driver
 *
 * @member succeeded Boolean that indicates if the request succeeded or not
 * @member value The value of the pin being read (if that was the request type)
 */
struct bea_gpio_request_response
{
  bool succeeded;
  bool value;
};

#endif
