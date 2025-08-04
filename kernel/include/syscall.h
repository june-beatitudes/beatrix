/**
 * @file
 * @brief Defines the functions that handle various system calls, along with
 * their numbers
 * @author Juniper Beatitudes <juniper@eyes-like-fire.org>
 * @date 2025-07-16
 */

#ifndef BEA_SYSCALL_H
#define BEA_SYSCALL_H

/**
 * @brief System call enumeration
 */
enum bea_syscall
{
  /// Send a message to the kernel log
  BEA_SYSCALL_LOG = 0,
  /// Load a file from the SD card and begin executing it (replacing the current
  /// process image)
  BEA_SYSCALL_EXECUTE,
  /// Find a driver from its name
  BEA_SYSCALL_FIND_DRIVER,
  /// Make a request from a kernel driver
  BEA_SYSCALL_DRIVER_RQ,
  /// Make a request to not be interrupted by the scheduler during an important
  /// task. Reserved for future use
  BEA_SYSCALL_SILENCE_RQ,
  /// Yield the rest of your allotted task time to the scheduler. Reserved for
  /// future use
  BEA_SYSCALL_YIELD,
  /// Request to spawn a new task
  BEA_SYSCALL_SPAWN,
  /// Request to kill either one of your child tasks or yourself
  BEA_SYSCALL_KILL,
  /// Get information about the current task
  BEA_SYSCALL_TASKINFO,
  BEA_NUM_SYSCALLS,
};

/**
 * @brief System call interrupt handler, fetches reason from instruction and
 * arguments from R0 and R1 respectively, return value is in R0
 */
void bea_do_syscall (void);

#endif
