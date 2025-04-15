#include <stdio.h>
#include <sys/types.h>

long ftello(FILE *stream) {
    return (long)ftell(stream);
}

int fseeko(FILE *stream, off_t offset, int whence) {
    return fseek(stream, (long)offset, whence);
}

