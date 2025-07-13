#include <stddef.h>
#include <string.h>

void
bea_strncpy (const char *from, char *to, size_t n)
{
  for (size_t i = 0; i < n; ++i)
    {
      to[i] = from[i];
    }
}
