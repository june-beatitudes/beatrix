#ifndef BEATRIX_VERSION_H
#define BEATRIX_VERSION_H

#include <common.h>

#define BEA_VERSION_NAME_MAXLEN 31
#define BEA_GIT_HASH_LEN 40

struct bea_version
{
  char name[BEA_VERSION_NAME_MAXLEN];
  char git_hash[BEA_GIT_HASH_LEN];
};

#endif