#ifndef BEA_STRING_H
#define BEA_STRING_H

#include <stddef.h>

/**
 * @brief Copies `n` characters from a character buffer
 *
 * @param from The buffer to copy from
 * @param to The buffer to copy into
 * @param n The number of characters to copy
 */
void bea_strncpy (const char *from, char *to, size_t n);

#endif
