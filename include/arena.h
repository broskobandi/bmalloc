#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct arena arena_t;

arena_t *arena_new(size_t size);
void arena_del(arena_t *arena);
void *arena_get_ptr(arena_t *arena, size_t index);

#endif
