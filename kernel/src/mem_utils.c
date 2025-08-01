#include <mem_utils.h>
#include <stddef.h>
#include <stdint.h>

__attribute__ ((always_inline)) inline int
memcmp (const void *p1, const void *p2, size_t n)
{
  size_t i;
  if (p1 == p2)
    {
      return 0;
    }
  for (i = 0; i < n; ++i)
    {
      if (((uint8_t *)p1)[i] != ((uint8_t *)p2)[i])
        {
          break;
        }
    }
  return (n == i) ? 0 : ((uint8_t *)p1)[i] - ((uint8_t *)p2)[i];
}

__attribute__ ((always_inline)) inline void *
memcpy (void *p1, void *p2, size_t n)
{
  for (size_t i = 0; i < n; ++i)
    {
      ((uint8_t *)p1)[i] = ((uint8_t *)p2)[i];
    }
  return p1;
}

__attribute__ ((always_inline)) inline void *
memmove (void *p1, void *p2, size_t n)
{
  if ((uint8_t *)p1 < (uint8_t *)p2)
    {
      return memcpy (p1, p2, n);
    }
  else if ((uint8_t *)p1 > (uint8_t *)p2)
    {
      for (size_t i = n; i > 0; --i)
        {
          ((uint8_t *)p1)[i - 1] = ((uint8_t *)p2)[i - 1];
        }
    }
  return p1;
}

__attribute__ ((always_inline)) inline void *
memset (void *dest, int ch, size_t count)
{
  uint8_t byte = (uint8_t)ch;
  for (size_t i = 0; i < count; ++i)
    {
      ((uint8_t *)dest)[i] = byte;
    }
  return dest;
}

__attribute__ ((always_inline)) inline char *
strchr (const char *str, int ch)
{
  while (*str != '\0')
    {
      if (ch == *str)
        {
          return str;
        }
      str++;
    }
  return NULL;
}

__attribute__ ((always_inline)) inline int
strcmp (const char *s1, const char *s2)
{

  if (s1 == s2)
    {
      return 0;
    }
  for (; *s1 != '\0' && *s2 != '\0'; ++s1, ++s2)
    {
      if (*s1 != *s2)
        {
          break;
        }
    }
  return (*s1 == *s2) ? 0 : *s1 - *s2;
}