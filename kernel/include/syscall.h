/**
 * @file
 * @brief Defines the functions that handle various system calls, along with
 * their numbers
 * @author Juniper Beatitudes
 * @date 2025-07-16
 */

#ifndef BEA_SYSCALL_H
#define BEA_SYSCALL_H

/**
 * @brief System call interrupt handler, fetches reason and argument from R0
 * and R1 respectively, return value is in R0
 */
void bea_do_syscall (void);

/**
 * @brief System call enumeration
 */
enum bea_syscall_t
{
  /// Send a message to the kernel log
  BEA_SYSCALL_LOG = 0,
  /// Make a request from a kernel driver
  BEA_SYSCALL_DRIVER_RQ,
  /// Make a request to not be interrupted by the scheduler during an important
  /// task
  BEA_SYSCALL_SILENCE_RQ,
  /// Yield the rest of your allotted task time to the scheduler
  BEA_SYSCALL_YIELD,
  /// Request new heap memory
  BEA_SYSCALL_MALLOC,
  /// Free already-allocated heap memory
  BEA_SYSCALL_FREE,
  /// Request new heap memory filled with zeroes
  BEA_SYSCALL_CALLOC,
  /// Request to reallocate a buffer
  BEA_SYSCALL_REALLOC,
  /// Request to spawn a new thread
  BEA_SYSCALL_SPAWN,
  /// Request to kill either one of your child threads or yourself
  BEA_SYSCALL_KILL,
  /// Number of syscall reasons (when cast to a `uint32_t`)
  BEA_NUM_SYSCALLS,
};

#endif
