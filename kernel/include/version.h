#ifndef BEATRIX_VERSION_H
#define BEATRIX_VERSION_H

#define BEA_VERSION_NAME_MAXLEN 31
#define BEA_GIT_HASH_LEN 40

/**
 * @brief A version of BEATRIX is uniquely given by its Git commit hash and the
 * major version name (conventionally the name of a Pigeon Pit song)
 *
 */
struct bea_version
{
  char name[BEA_VERSION_NAME_MAXLEN];
  char git_hash[BEA_GIT_HASH_LEN];
};

#endif
