/**
 * @file
 * @author Juniper Beatitudes <juniper@eyes-like-fire.org>
 * @brief Defines the `bea_task` structure and outlines basic task operations
 * @date 2025-07-26
 *
 */

#ifndef BEA_TASK_H
#define BEA_TASK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Defines the basic structure of a task/thread in the BEATRIX kernel
 *
 */
struct bea_task
{
  /// Determines if this represents a real task or a dead/not-yet-spawned one
  bool is_valid;
  /// Task ID, unique for the runtime of the system
  uint64_t id;

  /// Priority of a task (number of ticks it will be given to run in a row)
  uint32_t priority;
  /// Last system tick the task ran at
  uint64_t last_ran;
  /// Entrypoint of the task, including a void pointer argument
  void (*entry) (void *);

  /// Pointer to the buffer for the task stack
  uint32_t *stack;
  /// Size of the task stack (in 32-bit words)
  size_t stack_size;

  /// Pointer to the buffer for the task heap
  uint32_t *heap;
  /// Size of the task heap (in 32-bit words)
  size_t heap_size;
};

/// Maximum number of tasks that can be allocated into the task table
#define BEA_MAX_TASKS 256

/// Table (in min-heap form) containing all relevant tasks
struct bea_task bea_task_table[BEA_MAX_TASKS];

/**
 * @brief Runs the scheduler and branches to the next task
 *
 */
void bea_do_scheduler (void);

#endif