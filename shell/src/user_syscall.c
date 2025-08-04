#include <stdint.h>
#include <syscall.h>
#include <user_syscall.h>

uint32_t
shell_do_syscall_0arg (enum bea_syscall sc)
{
  uint32_t ret;
  __asm__ volatile ("MOV R0, %[sc]\n"
                    "SVC #0\n"
                    "MOV %[ret], R0\n"
                    : [ret] "=r"(ret)
                    : [sc] "r"(sc)
                    : "r0");
  return ret;
}

uint32_t
shell_do_syscall_1arg (enum bea_syscall sc, uint32_t arg0)
{
  uint32_t ret;
  __asm__ volatile ("MOV R0, %[sc]\n"
                    "MOV R1, %[arg0]\n"
                    "SVC #0\n"
                    "MOV %[ret], R0\n"
                    : [ret] "=r"(ret)
                    : [sc] "r"(sc), [arg0] "r"(arg0)
                    : "r0", "r1");
  return ret;
}

uint32_t
shell_do_syscall_2arg (enum bea_syscall sc, uint32_t arg0, uint32_t arg1)
{
  uint32_t ret;
  __asm__ volatile ("MOV R0, %[sc]\n"
                    "MOV R1, %[arg0]\n"
                    "MOV R2, %[arg1]\n"
                    "SVC #0\n"
                    "MOV %[ret], R0\n"
                    : [ret] "=r"(ret)
                    : [sc] "r"(sc), [arg0] "r"(arg0), [arg1] "r"(arg1)
                    : "r0", "r1");
  return ret;
}

uint32_t
shell_do_syscall_3arg (enum bea_syscall sc, uint32_t arg0, uint32_t arg1,
                       uint32_t arg2)
{
  uint32_t ret;
  __asm__ volatile (
      "MOV R0, %[sc]\n"
      "MOV R1, %[arg0]\n"
      "MOV R2, %[arg1]\n"
      "MOV R3, %[arg2]\n"
      "SVC #0\n"
      "MOV %[ret], R0\n"
      : [ret] "=r"(ret)
      : [sc] "r"(sc), [arg0] "r"(arg0), [arg1] "r"(arg1), [arg2] "r"(arg2)
      : "r0", "r1");
  return ret;
}