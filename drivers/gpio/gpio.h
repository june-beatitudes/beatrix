/**
 * @file
 * @brief Presents a driver interface to the GPIO functionality available on
 * the Flipper
 * @author Juniper Beatitudes <juniper@eyes-like-fire.org>
 * @date 2025-07-16
 */

#ifndef BEA_GPIO_H
#define BEA_GPIO_H

#include <processor.h>
#include <rcc.h>
#include <stdbool.h>

/**
 * @brief Type of userspace request sent to the driver
 *
 */
enum bea_gpio_rq_type
{
  /// Set the mode for a particular line
  BEA_GPIO_SET_MODE,
  /// Set the chosen alternate function for an alternate function mode line
  BEA_GPIO_SET_ALT_FUNC,
  /// Set the value on an output line
  BEA_GPIO_SET_VALUE,
  /// Read the value of an input line
  BEA_GPIO_READ_VALUE,
  /// Set the pull direction of a line (i.e., pull-up, pull-down, floating)
  BEA_GPIO_SET_PULLDIR,
};

/**
 * @brief GPIO banks on the Flipper, each numbered based on its registers'
 * locations in memory
 *
 */
enum bea_gpio_bank
{
  BEA_GPIO_BANK_A = 0x48000000,
  BEA_GPIO_BANK_B = 0x48000400,
  BEA_GPIO_BANK_C = 0x48000800,
  BEA_GPIO_BANK_D = 0x48000C00,
  BEA_GPIO_BANK_E = 0x48001000,
  BEA_GPIO_BANK_H = 0x48001C00,
};

/**
 * @brief A single GPIO line, associated with a particular bank and a particular
 * pin on that bank
 *
 */
struct bea_gpio_line
{
  enum bea_gpio_bank bank;
  uint8_t pin;
};

/**
 * @brief Types of GPIO pin modes, with the numbers associated encoding the
 * register contents that they are associated with
 *
 */
enum bea_gpio_pin_mode
{
  /// GPIO read
  BEA_GPIO_IN = 0x00,
  /// GPIO write
  BEA_GPIO_OUT = 0b01,
  /// GPIO alternate function (see STM32WB55xx datasheet for options)
  BEA_GPIO_ALT = 0b10,
  /// GPIO ADC connection
  BEA_GPIO_ANALOG = 0b11,
};

/**
 * @brief Pull direction for a single GPIO line
 *
 */
enum bea_gpio_pull_direction
{
  BEA_GPIO_DIR_NOPULL = 0b00,
  BEA_GPIO_DIR_PULLUP = 0b01,
  BEA_GPIO_DIR_PULLDOWN = 0b10,
};

/**
 * @brief Type of alternate function assigned to a GPIO line. Consult the
 * STM32WB55xx datasheet for details
 *
 */
enum bea_gpio_alt_func
{
  BEA_GPIO_AF_SYS_AF = 0,
  BEA_GPIO_AF_TIM1_2_LPTIM1,
  BEA_GPIO_AF_TIM1_2,
  BEA_GPIO_AF_SPI2_SAI2_TIM2,
  BEA_GPIO_AF_I2C1_3,
  BEA_GPIO_AF_SPI1_2,
  BEA_GPIO_AF_RF,
  BEA_GPIO_AF_USART1,
  BEA_GPIO_AF_LPUART1,
  BEA_GPIO_AF_TSC,
  BEA_GPIO_AF_USB_QUADSPI,
  BEA_GPIO_AF_LCD,
  BEA_GPIO_AF_COMP1_2_TIM1,
  BEA_GPIO_AF_SAI1,
  BEA_GPIO_AF_TIM2_16_17_LPTIM2,
  BEA_GPIO_AF_EVENTOUT,
};

/**
 * @brief Types of error that can be encountered during a GPIO interaction
 * (assuming type safety rules aren't violated)
 *
 */
enum bea_gpio_error
{
  /// No error encountered
  BEA_GPIO_ERROR_NONE,
  /// Reserved for future use
  BEA_GPIO_WOULD_BLOCK,
  /// Invalid value for a GPIO line
  BEA_GPIO_INVALID_LINE,
};

/**
 * @brief Request sent to the GPIO driver from userspace
 *
 */
struct bea_gpio_request_arg
{
  /// The type of request
  enum bea_gpio_rq_type type;
  /// The GPIO line in question
  struct bea_gpio_line line;
  union
  {
    /// The new value to be written to the line (for write operations)
    bool value;
    /// The new mode the line should be set to (for set-mode operations)
    enum bea_gpio_pin_mode mode;
    /// The new alternate function the line should be associated with (for
    /// relevant operations)
    uint8_t alt_func;
    /// The new pull direction to set the pin to (for set-pull-direction
    /// operations)
    enum bea_gpio_pull_direction pulldir;
  };
};

/**
 * @brief Response from the GPIO driver
 *
 */
struct bea_gpio_request_response
{
  /// Any error encountered during the operation
  enum bea_gpio_error err;
  /// The value of the GPIO line being read (for input operations)
  bool value;
};

#ifdef BEA_KERNEL_INTERNAL

/**
 * @brief Set the GPIO mode for a particular line
 *
 * @param line The line
 * @param mode The mode
 * @return enum bea_gpio_error indicating any error encountered
 */
enum bea_gpio_error bea_gpio_set_mode (struct bea_gpio_line line,
                                       enum bea_gpio_pin_mode mode);

/**
 * @brief Set the GPIO alternate function for a particular line
 *
 * @param line The line
 * @param alt_func The alternate function
 * @return enum bea_gpio_error indicating any error encountered
 */
enum bea_gpio_error bea_gpio_set_alt_func (struct bea_gpio_line line,
                                           enum bea_gpio_alt_func);

/**
 * @brief Set the GPIO output value for a particular line
 *
 * @param line The line
 * @param value The output value
 * @return enum bea_gpio_error indicating any error encountered
 */
enum bea_gpio_error bea_gpio_set_value (struct bea_gpio_line line, bool value);

/**
 * @brief Reads the GPIO input value for a particular line
 *
 * @param line The line
 * @param out Out parameter for the input value
 * @return enum bea_gpio_error indicating any error encountered
 */
enum bea_gpio_error bea_gpio_read_value (struct bea_gpio_line line, bool *out);

/**
 * @brief Set the GPIO pull direction for a particular line
 *
 * @param line The line
 * @param dir The pull direction
 * @return enum bea_gpio_error indicating any error encountered
 */
enum bea_gpio_error bea_gpio_set_pulldir (struct bea_gpio_line line,
                                          enum bea_gpio_pull_direction dir);

bool bea_gpio_initialize (void);
bool bea_gpio_deinitialize (void);
void bea_gpio_request (void *arg, void *result);

/// Entry in the kernel driver table
extern const struct bea_driver BEA_GPIO_DRIVER;

#endif

#endif
