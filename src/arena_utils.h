#ifndef ARENA_UTILS_H
#define ARENA_UTILS_H

#include "arena.h"
#include <stddef.h>
#include <stdalign.h>
#include <berror.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <string.h>

#ifndef BUFF_SIZE_CUSTOM
#define BUFF_SIZE 1024LU * 4
#else
#define BUFF_SIZE BUFF_SIZE_CUSTOM
#endif
#define MMAP(size)\
	mmap(NULL, (size), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

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
	alignas(max_align_t) unsigned char buff[BUFF_SIZE];
	size_t offset;
	storage_t *next;
	storage_t *prev;
	ptr_list_t ptr_list;
};

struct arena {
	storage_t head;
	storage_t *tail;
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

#endif
