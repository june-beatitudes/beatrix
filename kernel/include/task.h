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
 * @brief State of a task as it relates to the scheduler
 *
 */
enum bea_task_state
{
  /// Dead, can be overwritten with another task as needed
  BEA_TASK_DEAD,
  /// Can be interrupted (has not requested silence)
  BEA_TASK_INTERRUPTIBLE,
  /// Cannot be interrupted (has requested silence)
  BEA_TASK_UNINTERRUPTIBLE,
};

/**
 * @brief State of the processor (frozen as part of a task object when it is
 * interrupted and restored when it begins execution again)
 *
 */
struct bea_processor_state
{
  /// R0-R12
  uint32_t general_regs[13];
  /// S0-S15
  float fp_regs[16];
  /// SP
  uint32_t stack_pointer;
  /// LR
  uint32_t link_register;
  /// PC
  uint32_t program_counter;
  /// ASPR (Application Program Status Register)
  uint32_t aspr;
};

/**
 * @brief Defines the basic structure of a task/thread in the BEATRIX kernel
 *
 */
struct bea_task
{
  /// Current state of the task
  enum bea_task_state state;
  /// Index in the task table, uniquely identifies it for the lifetime of the
  /// task
  uint32_t ind;
  /// Index of its direct parent
  uint32_t parent_ind;
  /// Priority of a task (number of ticks it will be given to run in a row)
  uint32_t priority;

  /// Pointer to the buffer for the task stack
  uint32_t *stack;
  /// Size of the task stack (in 32-bit words)
  size_t stack_size;

  /// Pointer to the buffer for the task heap
  uint32_t *heap;
  /// Size of the task heap (in 32-bit words)
  size_t heap_size;

  /// Last state the processor was in before the task was interrupted
  struct bea_processor_state last_state;
};

/// Maximum number of tasks that can be allocated into the task table
#define BEA_MAX_TASKS 256
/// "Parent index" of the root task
#define BEA_PARENTLESS_INDEX (BEA_MAX_TASKS + 1)

/**
 * @brief Runs the scheduler and branches to the next task as necessary
 *
 */
void bea_do_scheduler (void);

/**
 * @brief Spawns a new task
 *
 * @param priority The number of ticks the task will be given to run before it
 * is pre-empted. Must be less than its parent's priority, and its priority will
 * be subtracted from its parent's priority
 * @param entry The entrypoint function of the task
 * @param arg The argument to be passed to the entrypoint function
 * @param stack A buffer to use as the task's stack
 * @param stack_size The size of the task's stack
 * @param heap A buffer to use as the task's heap
 * @param heap_size The size of the task's heap
 * @param parent_ind The index of the task's parent
 * @return uint64_t The task's index in the task table
 */
uint32_t bea_spawn_task (uint32_t priority, void (*entry) (void *), void *arg,
                         uint32_t *stack, size_t stack_size, uint32_t *heap,
                         size_t heap_size, uint32_t parent_ind);

/**
 * @brief Kills a task (that is one of the children of the current task)
 *
 * @param task_ind The index of the task to kill
 */
void bea_kill_task (uint32_t task_ind);

#endif