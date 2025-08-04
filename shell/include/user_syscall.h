#include <stdint.h>
#include <syscall.h>

uint32_t shell_do_syscall_0arg (enum bea_syscall sc);

uint32_t shell_do_syscall_1arg (enum bea_syscall sc, uint32_t arg0);

uint32_t shell_do_syscall_2arg (enum bea_syscall sc, uint32_t arg0,
                                uint32_t arg1);

uint32_t shell_do_syscall_3arg (enum bea_syscall sc, uint32_t arg0,
                                uint32_t arg1, uint32_t arg2);