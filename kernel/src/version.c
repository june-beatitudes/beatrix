#include <common.h>
#include <version.h>

#ifndef BEA_GIT_HASH
#warning "Git hash not found -- something's wrong with the build system"
#define BEA_GIT_HASH "No Git hash-is the build system broken?"
#endif

__attribute__((section(".beatrix_version"), unused)) const struct bea_version BEATRIX_VERSION = {
    .name = "Love Letters",
    .git_hash = BEA_GIT_HASH,
};