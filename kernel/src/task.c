#include <register_utils.h>
#include <stddef.h>
#include <stdint.h>
#include <task.h>

static uint64_t highest_task_num = 0;
static uint32_t num_tasks = 0;

static struct bea_task bea_task_table[BEA_MAX_TASKS];
static uint32_t current_task_ind = 0;

__attribute__ ((noreturn)) void
bea_do_scheduler (void)
{
  // First, we save the current state
  uint32_t *stack_pointer;
  __asm__ volatile ("MOV [sp], SP" : [sp] "=r"(stack_pointer));
  struct bea_task current = bea_task_table[current_task_ind];
  __asm__ volatile ("MOV [r4], R4\n"
                    "MOV [r5], R5\n"
                    "MOV [r6], R6\n"
                    "MOV [r7], R7\n"
                    "MOV [r8], R8\n"
                    "MOV [r9], R9\n"
                    "MOV [r10], R10\n"
                    "MOV [r11], R11\n"
                    : [r4] "=r"(current.last_state.general_regs[4]),
                      [r5] "=r"(current.last_state.general_regs[5]),
                      [r6] "=r"(current.last_state.general_regs[6]),
                      [r7] "=r"(current.last_state.general_regs[7]),
                      [r8] "=r"(current.last_state.general_regs[8]),
                      [r9] "=r"(current.last_state.general_regs[9]),
                      [r10] "=r"(current.last_state.general_regs[10]),
                      [r11] "=r"(current.last_state.general_regs[11]));
  for (size_t i = 0; i < 4; ++i)
    {
      current.last_state.general_regs[i] = stack_pointer[i];
    }
  current.last_state.general_regs[12] = stack_pointer[4];
  current.last_state.link_register = stack_pointer[5];
  current.last_state.program_counter = stack_pointer[6];
  current.last_state.aspr = stack_pointer[7];
  for (size_t i = 0; i < 16; ++i)
    {
      current.last_state.fp_regs[i] = ((float *)stack_pointer)[i + 8];
    }

  // Then select the next task
  for (size_t i = 0; i < BEA_MAX_TASKS; ++i)
    {
      current_task_ind++;
      current_task_ind %= BEA_MAX_TASKS;
      if (bea_task_table[current_task_ind].state != BEA_TASK_DEAD)
        {
          break;
        }
    }
  current = bea_task_table[current_task_ind];

  // And load its state
  __asm__ volatile ("MOV R4, [r4]\n"
                    "MOV R5, [r5]\n"
                    "MOV R6, [r6]\n"
                    "MOV R7, [r7]\n"
                    "MOV R8, [r8]\n"
                    "MOV R9, [r9]\n"
                    "MOV R10, [r10]\n"
                    "MOV R11, [r11]\n"
                    :
                    : [r4] "r"(current.last_state.general_regs[4]),
                      [r5] "r"(current.last_state.general_regs[5]),
                      [r6] "r"(current.last_state.general_regs[6]),
                      [r7] "r"(current.last_state.general_regs[7]),
                      [r8] "r"(current.last_state.general_regs[8]),
                      [r9] "r"(current.last_state.general_regs[9]),
                      [r10] "r"(current.last_state.general_regs[10]),
                      [r11] "r"(current.last_state.general_regs[11])
                    : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10",
                      "r11");
  for (size_t i = 0; i < 4; ++i)
    {
      stack_pointer[i] = current.last_state.general_regs[i];
    }
  stack_pointer[12] = current.last_state.general_regs[12];
  stack_pointer[5] = current.last_state.link_register;
  stack_pointer[6] = current.last_state.program_counter;
  stack_pointer[7] = current.last_state.aspr;
  for (size_t i = 0; i < 16; ++i)
    {
      ((float *)stack_pointer)[i + 8] = current.last_state.fp_regs[i];
    }

  // Set up the SysTick timer
  uint32_t *STK_CTRL = (uint32_t *)0xE000E010;
  uint32_t *STK_LOAD = (uint32_t *)0xE000E014;
  bea_set_reg_bits (STK_LOAD, 23, 0, current.priority);
  bea_set_reg_bits (STK_CTRL, 1, 0, 0b11);

  // Branch away
  __asm__ volatile ("LDR LR, 0xFFFFFFED");
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
        }
    }
  bea_task_table[cursor].ind = cursor;
  bea_task_table[cursor].parent_ind = parent_ind;
  bea_task_table[cursor].last_state.stack_pointer
      = (uint32_t)stack + stack_size;
  bea_task_table[cursor].last_state.program_counter = (uint32_t)entry;
  bea_task_table[cursor].last_state.general_regs[0] = (uint32_t)arg;
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