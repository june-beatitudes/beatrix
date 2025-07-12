#ifndef BEA_FTOA_H
#define BEA_FTOA_H

#include <stdbool.h>
#include <stddef.h>

enum bea_ftoa_result
{
  BEA_FTOA_SUCCESS,
  BEA_FTOA_ENOSPACE,
  BEA_FTOA_WPRECISIONLOSS,
};

enum bea_ftoa_result bea_ftoa (float x, char *buf, size_t n_available,
                               bool include_positive_sign);

#endif
