#ifndef ARENA_UTILS_H
#define ARENA_UTILS_H

#include "arena.h"
#include <stddef.h>
#include <stdalign.h>
#include <berror.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <string.h>

#ifndef STORAGE_SIZE_CUSTOM
#define STORAGE_SIZE 1024LU * 4
#else
#define STORAGE_SIZE STORAGE_SIZE_CUSTOM
#endif
#define MMAP(size)\
	mmap(NULL, (size), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
#define FREE_PTR_SIZE_A STORAGE_SIZE / 4
#define FREE_PTR_SIZE_B STORAGE_SIZE / 2
#define FREE_PTR_SIZE_C STORAGE_SIZE / 2 + STORAGE_SIZE / 4 

typedef struct ptr ptr_t;
typedef struct ptr_list ptr_list_t;
typedef struct storage storage_t;

struct ptr {
	storage_t *storage;
	void *data;
	size_t size;
	ptr_t *next;
	ptr_t *prev;
	bool is_valid;
};

struct ptr_list {
	ptr_t head;
	ptr_t *tail;
};

struct storage {
	unsigned char *buff;
	size_t offset;
	storage_t *next;
	storage_t *prev;
	ptr_list_t ptr_list;
};

struct arena {
	storage_t head;
	storage_t *tail;
	size_t size;
	// ptr_list_t free_list_a;
	// ptr_list_t free_list_b;
	// ptr_list_t free_list_c;
	// ptr_list_t free_list_d;
	size_t num_free_ptrs;
};

static inline size_t round_up(size_t size) {
	if (!size) ERR("size mustn't be 0.", (size_t)-1);
	return ((size + alignof(max_align_t) - 1) & ~(alignof(max_align_t) - 1));
}

static inline size_t total_size(size_t size) {
	if (!size) ERR("size mustn't be 0.", (size_t)-1);
	return round_up(sizeof(ptr_t)) + round_up(size);
}

static inline int arena_expand(arena_t *arena) {
	if (!arena) ERR("Invalid argument.", 1);
	arena->tail->next = MMAP(sizeof(storage_t));
	if (!arena->tail->next)
		ERR("Failed to allocate new storage", 1);
	arena->tail->next->prev = arena->tail;
	arena->tail = arena->tail->next;
	arena->tail->next = NULL;
	arena->tail->offset = 0;
	memset(&arena->tail->ptr_list, 0, sizeof(ptr_list_t));
	arena->tail->ptr_list.tail = &arena->tail->ptr_list.head;
	return 0;
}

static inline void *alloc_with_mmap(size_t size) {
	if (!size) ERR("size mustn't be 0.", NULL);
	ptr_t *ptr = MMAP(total_size(size));
	if (!ptr) ERR("Failed to allocate pointer with mmap().", NULL);
	memset(ptr, 0, sizeof(ptr_t));
	ptr->is_valid = true;
	ptr->data = (char*)ptr + round_up(sizeof(ptr_t));
	ptr->size = size;
	return ptr->data;
}

static inline void *alloc_in_arena(arena_t *arena, size_t size) {
	if (!arena || !size) ERR("Invalid argument.", NULL);
	if (arena->tail->offset + total_size(size) > arena->size)
		if (arena_expand(arena)) ERR("Failed to expand arena.", NULL);
	if (arena->tail->ptr_list.tail->is_valid) {
		arena->tail->ptr_list.tail->next =
			(ptr_t*)&arena->tail->buff[arena->tail->offset];
		memset(arena->tail->ptr_list.tail->next, 0, sizeof(ptr_t));
		arena->tail->ptr_list.tail->next->prev =
			arena->tail->ptr_list.tail;
		arena->tail->ptr_list.tail =
			arena->tail->ptr_list.tail->next;
	}
	ptr_t *ptr = arena->tail->ptr_list.tail;
	ptr->size = size;
	ptr->is_valid = true;
	ptr->storage = arena->tail;
	ptr->data = (char*)ptr + round_up(sizeof(ptr_t));
	return ptr->data;
}

static inline void *use_free_ptr(arena_t *arena, size_t size) {
	if (!arena || !size) ERR("Invalid argument.", NULL);
	storage_t *cur_storage = &arena->head;
	while (cur_storage) {
		ptr_t *cur_ptr = &cur_storage->ptr_list.head;
		while (cur_ptr) {
			if (
				!cur_ptr->is_valid &&
				cur_ptr->size >= size &&
				cur_ptr->size < size * 2
			) {
				arena->num_free_ptrs--;
				cur_ptr->is_valid = true;
				return cur_ptr->data;
			}
			cur_ptr = cur_ptr->next;
		}
		cur_storage = cur_storage->next;
	}
	ERR("Failed to find free ptr.", NULL);
}

#endif
