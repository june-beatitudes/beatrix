#ifndef BEA_KERN_COMMON_H
#define BEA_KERN_COMMON_H

typedef int bool_t;
#define TRUE 0x1
#define FALSE 0X0

typedef unsigned int size_t;
typedef signed int ssize_t;

/**
* Half of these are stolen from POSIX errno.h
*/
typedef enum
{
        BEA_ENONE,
        BEA_EINVAL,
        BEA_EUNAVAILABLE,
        BEA_EBUSY,
        BEA_EREFUSED,
        BEA_EEXIST,
        BEA_ENOMEM,
        BEA_EPERM,
        BEA_ENOEXEC,
} bea_err_t;

#define NULL 0x0

#endif