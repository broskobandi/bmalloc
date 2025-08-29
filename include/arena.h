#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct arena arena_t;

arena_t *arena_new();
void *arena_alloc(arena_t *arena, size_t size);
void arena_dealloc(void *ptr);
void arena_del(arena_t *arena);

#endif
