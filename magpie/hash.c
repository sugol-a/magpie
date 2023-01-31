#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#define XXH_INLINE_ALL 1
#include <magpie/external/xxHash/xxhash.h>

#include <magpie/hash.h>

uint64_t
hash_str(const void* a)
{
    const char* str = *(const char**)a;
    const size_t len = strlen(str);
    
    return XXH64(str, len, 0);
}
