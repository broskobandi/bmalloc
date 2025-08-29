#include "test_arena.h"
#include "arena.h"
#include "arena_utils.h"
#include <string.h>

void test_arena_new() {
	{ // Normal case
		arena_t *arena = arena_new();
		BASSERT(arena->tail == &arena->head);
		BASSERT(arena->tail->ptr_list.tail == &arena->tail->ptr_list.head);
	}
}

void test_round_up() {
	{ // Normal case
		BASSERT(round_up(alignof(max_align_t) / 2) == alignof(max_align_t));
	}
	{ // size 0
		BASSERT(round_up(0) == (size_t)-1);
		BASSERT(!strcmp(berror_get().msg, "size mustn't be 0."));
	}
}

void test_total_size() {
	{ // Normal case
		size_t size = 5;
		size_t expected_total_size = round_up(sizeof(ptr_t)) + round_up(size);
		BASSERT(expected_total_size == total_size(size));
	}
	{ // Normal case
		BASSERT(total_size(0) == (size_t)-1);
		BASSERT(!strcmp(berror_get().msg, "size mustn't be 0."));
	}
}

void test_arena_expand() {
	{ // Normal case
		arena_t *arena = arena_new();
		BASSERT(!arena->head.next);
		BASSERT(!arena_expand(arena));
		BASSERT(arena->head.next);
	}
	{ // Invalid argument
		BASSERT(arena_expand(NULL));
		BASSERT(!strcmp(berror_get().msg, "Invalid argument."));
	}
}
