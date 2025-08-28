#ifndef BMALLOC_H
#define BMALLOC_H

#include <stddef.h>

void *bmalloc(size_t size);
void bfree(void *ptr);
void *brealloc(void *ptr, size_t size);

#endif
