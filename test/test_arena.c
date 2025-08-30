#include "test_arena.h"
#include "arena.h"
#include "arena_utils.h"
#include <string.h>

void test_roundup() {
	{ // to page
		size_t page = (size_t)getpagesize();
		size_t size = 3;
		ASSERT(rounduptopage(size) == page);
	} // to max align
	{
		size_t align = alignof(max_align_t);
		size_t size = 3;
		ASSERT(rounduptoalign(size) == align);
	}
}

void test_arena_new() {
	{ // Normal case
		size_t size = 10;
		size_t unit = 4;
		arena_t *arena = arena_new(size, unit);
		ASSERT(arena);
		void *buff = (char*)arena + ((sizeof(arena_t) + alignof(max_align_t) - 1) & ~(alignof(max_align_t) - 1));
		ASSERT(buff == arena->storage.buff);
		size_t expected_buff_size = rounduptopage(rounduptoalign(sizeof(arena_t)) + size * unit) - rounduptoalign(sizeof(arena_t));
		ASSERT(arena->buff_size == expected_buff_size);
	}
	{ // No size
		ASSERT(!arena_new(0, 4));
		ASSERT(!strcmp(error_get().msg, "Invalid argument."));
	}
	{ // unit not power of 2
		ASSERT(!arena_new(16, 3));
		ASSERT(!strcmp(error_get().msg, "unit must be a power of 2."));
	}
}
