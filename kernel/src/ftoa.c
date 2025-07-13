#include <ftoa.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

enum bea_ftoa_result
bea_ftoa (float x, char *buf, size_t n_available, bool include_positive_sign)
{
  if (signbit (x) == 0)
    {
      if (include_positive_sign)
        {
          buf[0] = '+';
          buf++;
          n_available--;
        }
    }
  else
    {
      buf[0] = '-';
      buf++;
      n_available--;
    }

  size_t integral_length = lroundf (ceilf (log10f (x)));
  if (integral_length > n_available)
    {
      return BEA_FTOA_ENOSPACE;
    }
  else if (integral_length < n_available)
    {
      buf[integral_length] = '.'; // Print at least a decimal point
    }
  size_t fractional_length = (n_available == integral_length)
                                 ? 0
                                 : (n_available - integral_length - 1);

  float abs = fabsf (x);
  float integral_part;
  float fractional_part = modff (abs, &integral_part);

  char *cursor = buf;
  for (int32_t i = integral_length - 1; i >= 0; --i)
    {
      cursor[i] = lroundf (fmodf (integral_part, 10.0f)) + '0';
      integral_part = floorf (integral_part / 10.0f);
    }

  cursor += integral_length + 1;
  for (size_t i = 0; i < fractional_length; ++i)
    {
      fractional_part *= 10.0f;
      uint8_t digit = (uint8_t)lroundf (floorf (fractional_part));
      fractional_part -= floorf (fractional_part);
      cursor[i] = '0' + digit;
    }
  return true;
}
