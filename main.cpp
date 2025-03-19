#include <string.h>

void __aeabi_memmov(void *dest, const void *src, size_t n) {
    memmove(dest, src, n);
}
