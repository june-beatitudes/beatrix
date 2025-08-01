#include <processor.h>
#include <register_utils.h>
#include <stddef.h>
#include <stdint.h>
#include <task.h>

static uint32_t num_tasks;
struct bea_task bea_task_table[BEA_MAX_TASKS];
static uint32_t current_task_ind;

__attribute__ ((used)) uint32_t
_bea_do_ctx_switch (uint32_t *to_save)
{
  for (size_t i = 0; i < 33; ++i)
    {
      bea_task_table[current_task_ind].last_state.regs[i] = to_save[i];
    }
  bea_task_table[current_task_ind].last_state.regs[BEA_PC_IND] |= 1;
  for (size_t i = 0; i < BEA_MAX_TASKS; ++i)
    {
      current_task_ind++;
      current_task_ind %= BEA_MAX_TASKS;
      if (bea_task_table[current_task_ind].state != BEA_TASK_DEAD)
        {
          break;
        }
    }
  return bea_task_table[current_task_ind].last_state.stack_pointer;
}

// Includes R0-3, R12, PC, LR, XSPR, S0-S15, FSPCR, and spacer
#define STACK_PARTIAL_LOAD_BYTES 104

__attribute__ ((used)) uint32_t *
_bea_save_psp (uint32_t *psp)
{
  bea_task_table[current_task_ind].last_state.stack_pointer
      = (uint32_t)psp + STACK_PARTIAL_LOAD_BYTES;
  return psp;
}

#undef STACK_PARTIAL_LOAD_BYTES

__attribute__ ((used)) uint32_t *
_bea_fill_in_state (uint32_t *psp)
{
  for (size_t i = 0; i < 33; ++i)
    {
      psp[i] = bea_task_table[current_task_ind].last_state.regs[i];
    }
  return psp;
}

__attribute__ ((used)) uint32_t
_bea_get_priority (void)
{
  return bea_task_table[current_task_ind].priority;
}

// Defined in scheduler_direct.S (best written in straight assembler)
extern void bea_do_scheduler (void) __asm__ ("bea_do_scheduler");

void
bea_init_scheduler ()
{
  num_tasks = 0;
  current_task_ind = 0;
  for (size_t i = 0; i < BEA_MAX_TASKS; ++i)
    {
      bea_task_table[i].state = BEA_TASK_DEAD;
    }
}

uint32_t
bea_spawn_task (uint32_t priority, void (*entry) (void *), void *arg,
                uint32_t *stack, size_t stack_size, uint32_t *heap,
                size_t heap_size, uint32_t parent_ind)
{
  // Figure out if we can even spawn it
  if (num_tasks >= BEA_MAX_TASKS)
    {
      return ~0;
    }
  if (parent_ind != BEA_PARENTLESS_INDEX)
    {
      struct bea_task par = bea_task_table[parent_ind];
      if (stack < par.heap || (stack + stack_size) > (par.heap + par.heap_size))
        {
          // Trying to give away space you don't have
          return ~0;
        }
      else if (heap < par.heap
               || (heap + heap_size) > (par.heap + par.heap_size))
        {
          // Ditto
          return ~0;
        }
      else if (par.priority <= priority)
        {
          // Trying to give away priority you don't have
          return ~0;
        }
      else if ((heap < stack && (heap + heap_size) > stack)
               || (stack < heap && (stack + stack_size) > heap))
        {
          // Overlapping stack and heap
          return ~0;
        }
      bea_task_table[parent_ind].priority -= priority;
    }
  struct bea_task new = {
    .heap = heap,
    .heap_size = heap_size,
    .stack = stack,
    .stack_size = stack_size,
    .state = BEA_TASK_INTERRUPTIBLE,
    .priority = priority,
    .parent_ind = parent_ind,
  };
  size_t cursor = 0;
  for (size_t i = 0; i < BEA_MAX_TASKS; ++i, ++cursor)
    {
      cursor %= BEA_MAX_TASKS;
      if (bea_task_table[cursor].state == BEA_TASK_DEAD)
        {
          bea_task_table[cursor] = new;
          break;
        }
    }
  bea_task_table[cursor].ind = cursor;
  bea_task_table[cursor].parent_ind = parent_ind;
  bea_task_table[cursor].last_state.stack_pointer
      = (uint32_t)stack + stack_size;
  bea_task_table[cursor].last_state.regs[BEA_PC_IND] = (uint32_t)(entry);
  bea_task_table[cursor].last_state.regs[BEA_R0_IND] = (uint32_t)(arg);
  bea_task_table[cursor].last_state.regs[BEA_LR_IND] = ~0;
  bea_task_table[cursor].last_state.regs[BEA_ASPR_IND] = 1 << 24;
  num_tasks++;
  return cursor;
}

void
bea_kill_task (uint32_t task_ind)
{
  num_tasks--;
  for (size_t i = 0; i < BEA_MAX_TASKS; ++i)
    {
      if (bea_task_table[i].parent_ind == task_ind
          && bea_task_table[i].state != BEA_TASK_DEAD)
        {
          bea_kill_task (i);
        }
    }
  if (num_tasks == 0)
    {
      // TODO: Add handler where we execute the shell
    }
  bea_task_table[task_ind].state = BEA_TASK_DEAD;
  for (size_t i = 0; i < BEA_MAX_TASKS; ++i)
    {
      if (bea_task_table[current_task_ind].state != BEA_TASK_DEAD)
        {
          break;
        }
      current_task_ind++;
      current_task_ind %= BEA_MAX_TASKS;
    }
}