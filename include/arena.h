#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct arena arena_t;

#define TYPEDEF_ARENA(T)\
	typedef struct arena_##T arena_##T##_t;\
	static inline arena_##T##_t *arena_##T##_new(size_t size) {\
		return (arena_##T##_t*)arena_new(size, sizeof(T));\
	}\
	static inline T *arena_##T##_alloc(arena_##T##_t *arena, size_t len) {\
		return (T*)arena_alloc((arena_t*)arena, sizeof(T), len);\
	}\
	static inline void arena_##T##_dealloc(arena_##T##_t *arena) {\
		arena_dealloc((arena_t*)arena);\
	}\
	static inline void arena_##T##_del(arena_##T##_t *arena) {\
		arena_del((arena_t*)arena);\
	}\
	static inline T *arena_##T##_realloc(arena_##T##_t *arena, T *ptr, size_t size) {\
		return arena_realloc((arena_t*)arena, (void*)ptr, size, sizeof(T));\
	}

arena_t *arena_new(size_t size, size_t unit);
void *arena_alloc(arena_t *arena, size_t unit, size_t len);
void arena_dealloc(void *ptr);
void arena_del(arena_t *arena);
void *arena_realloc(arena_t *arena, void *ptr, size_t len, size_t unit);

#endif
