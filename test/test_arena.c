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
	{ // size 0
		BASSERT(total_size(0) == (size_t)-1);
		BASSERT(!strcmp(berror_get().msg, "size mustn't be 0."));
	}
}

void test_arena_expand() {
	{ // Normal case
		arena_t *arena = arena_new();
		storage_t *old_tail = arena->tail;
		BASSERT(!arena->head.next);
		BASSERT(!arena_expand(arena));
		BASSERT(arena->head.next);
		BASSERT(arena->tail == old_tail->next);
		BASSERT(&arena->head == arena->tail->prev);
	}
	{ // Invalid argument
		BASSERT(arena_expand(NULL));
		BASSERT(!strcmp(berror_get().msg, "Invalid argument."));
	}
}

void test_alloc_with_mmap() {
	{ // Normal case
		size_t size = 16;
		void *data = alloc_with_mmap(size);
		BASSERT(data);
		ptr_t *ptr = (ptr_t*)((char*)data - round_up(sizeof(ptr_t)));
		BASSERT(ptr->is_valid);
		BASSERT(ptr->data == data);
		BASSERT(ptr->size == size);
	}
	{ // size is 0
		BASSERT(!alloc_with_mmap(0));
		BASSERT(!strcmp(berror_get().msg, "size mustn't be 0."));
	}
}

void test_alloc_in_arena() {
	{ // Normal case
		arena_t *arena = arena_new();
		size_t size = 16;
		void *data = alloc_in_arena(arena, size);
		BASSERT(data);
		ptr_t *ptr = (ptr_t*)((char*)data - round_up(sizeof(ptr_t)));
		BASSERT(ptr->data == data);
		BASSERT(ptr->size == size);
		BASSERT(ptr->storage == arena->tail);
	}
}

void test_use_free_ptr() {
	{ // Normal case
		arena_t *arena = arena_new();
		size_t size = 16;
		void *original_data = arena_alloc(arena, size);
		BASSERT(original_data);
		arena->num_free_ptrs = 1;
		arena->tail->ptr_list.tail->is_valid = false;
		void *new_data = use_free_ptr(arena, size);
		BASSERT(new_data);
		BASSERT(new_data == original_data);
	}
}
