#include "test_arena.h"
#include "arena.h"
#include "arena_utils.h"
#include <string.h>

void test_arena_new() {
	{ // Normal case
		size_t size = 16;
		arena_t *arena = arena_new(size);
		ASSERT(arena);
		ASSERT(arena->data == (unsigned char*)arena + roundup(sizeof(arena_t), alignof(max_align_t)));
		ASSERT(arena->size == roundup(size, (size_t)getpagesize()));
	}
}

void test_arena_del() {
	{ // Normal case
		error_reset();
		arena_t *arena = arena_new(16);
		arena_del(arena);
		arena = NULL;
		ASSERT(!error_get().msg);
		ASSERT(!error_get().file);
		ASSERT(!error_get().func);
		ASSERT(!error_get().line);
	}
}

void test_arena_get_ptr() {
	{ // Normal case
		size_t size = 16;
		arena_t *arena = arena_new(size);
		size_t index = 5;
		void *ptr = arena_get_ptr(arena, index);
		ASSERT(ptr);
		int value = 5;
		memcpy(ptr, &value, sizeof(int));
		ASSERT(!memcmp(ptr, (char*)arena + roundup(sizeof(arena_t), alignof(max_align_t)) + index, sizeof(int)));
		ASSERT(*(int*)arena_get_ptr(arena, index) == value);
	}
	{ // out of bounds
		size_t size = 16;
		arena_t *arena = arena_new(size);
		ASSERT(!arena_get_ptr(arena, roundup(size, (size_t)getpagesize()) + 1));
		printf("%lu\n", arena->size);
		ASSERT(!strcmp(error_get().msg, "index is too big."));
	}
}
