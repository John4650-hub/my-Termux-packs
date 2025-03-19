#include <string.h>

// Custom implementation of __aeabi_memmov
void __aeabi_memmov(void *dest, const void *src, size_t n) {
    memmove(dest, src, n);
}
